#include "ProxyManager.h"
#include "WhiteBlackList.h"
#include "config.h"
#include "logger.h"
#include <algorithm>
#include <cctype>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <mutex>

namespace
{
	static auto ReadTextFile(const std::string& path) -> std::string
	{
		std::ifstream file(path, std::ios::binary);
		if (!file.is_open())
		{
			return {};
		}
		return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
	}

	static auto WriteTextFile(const std::string& path, const std::string& content) -> void
	{
		std::filesystem::path file_path(path);
		if (!file_path.parent_path().empty())
		{
			std::filesystem::create_directories(file_path.parent_path());
		}
		std::ofstream file(path, std::ios::binary | std::ios::trunc);
		file.write(content.data(), static_cast<std::streamsize>(content.size()));
	}

	static auto ClampPort(int value, const char* field) -> std::uint16_t
	{
		if (value < 1 || value > 65535)
		{
			throw ProxyException(std::string("Invalid ") + field + " value");
		}
		return static_cast<std::uint16_t>(value);
	}

	static auto BuildView(const ProxyServiceConfig& config, const std::shared_ptr<Proxy>& proxy, bool is_default) -> ProxyServiceView
	{
		ProxyServiceView view;
		view.id = config.id;
		view.name = config.name;
		view.local_address = config.local_address;
		view.local_port = config.local_port;
		view.remote_address = config.remote_address;
		view.remote_port = config.remote_port;
		view.max_player = proxy ? proxy->GetMaxPlayer() : config.max_player;
		view.motd_path = config.motd_path;
		view.running = static_cast<bool>(proxy);
		view.is_default = is_default;
		view.start_time = proxy ? proxy->GetStartTime() : 0;
		view.online_users = proxy ? static_cast<std::uint32_t>(proxy->GetUsersInfo().size()) : 0;
		view.default_proxy = config.remote_address + ":" + std::to_string(config.remote_port);
		return view;
	}
}

ProxyManager::~ProxyManager() noexcept
{
	std::map<std::string, ProxyRecord> records_to_destroy;
	{
		std::unique_lock lock(this->mutex);
		this->log_sink = {};
		records_to_destroy.swap(this->records);
		this->order.clear();
	}
}

void ProxyManager::SetStorePath(std::string path)
{
	std::unique_lock lock(this->mutex);
	this->store_path = std::move(path);
}

auto ProxyManager::GetStorePath() const -> std::string
{
	std::shared_lock lock(this->mutex);
	return this->store_path;
}

void ProxyManager::SetLogSink(LogSink sink)
{
	std::unique_lock lock(this->mutex);
	this->log_sink = std::move(sink);
}

auto ProxyManager::NormalizeId(std::string id) -> std::string
{
	for (char& ch : id)
	{
		if (!(std::isalnum(static_cast<unsigned char>(ch)) || ch == '_' || ch == '-'))
		{
			ch = '_';
		}
	}
	while (!id.empty() && id.front() == '_')
	{
		id.erase(id.begin());
	}
	while (!id.empty() && id.back() == '_')
	{
		id.pop_back();
	}
	return id;
}

auto ProxyManager::MakeDefaultName(const ProxyServiceConfig& service) -> std::string
{
	if (!service.name.empty())
	{
		return service.name;
	}
	return service.remote_address + ":" + std::to_string(service.remote_port);
}

auto ProxyManager::GenerateId(const std::map<std::string, ProxyRecord>& records, const ProxyServiceConfig& service) -> std::string
{
	std::string base = NormalizeId(service.id);
	if (base.empty())
	{
		base = NormalizeId(service.name);
	}
	if (base.empty())
	{
		base = "proxy_" + std::to_string(service.local_port);
	}
	if (records.find(base) == records.end())
	{
		return base;
	}
	for (std::size_t index = 2;; ++index)
	{
		std::string candidate = base + "_" + std::to_string(index);
		if (records.find(candidate) == records.end())
		{
			return candidate;
		}
	}
}

void ProxyManager::ValidateConfig(const ProxyServiceConfig& service)
{
	if (service.local_address.empty())
	{
		throw ProxyException("Local address is empty");
	}
	if (service.remote_address.empty())
	{
		throw ProxyException("Remote address is empty");
	}
	if (service.local_port == 0)
	{
		throw ProxyException("Local port is invalid");
	}
	if (service.remote_port == 0)
	{
		throw ProxyException("Remote port is invalid");
	}
}

void ProxyManager::EmitLog(const std::string& proxy_id, const std::string& message) const
{
	LogSink sink;
	{
		std::shared_lock lock(this->mutex);
		sink = this->log_sink;
	}
	if (sink)
	{
		sink(proxy_id, message);
	}
}

void ProxyManager::AttachCallbacks(const std::string& proxy_id, const std::shared_ptr<Proxy>& proxy)
{
	proxy->log_output += [this, proxy_id](const char* message) {
		if (message)
		{
			this->EmitLog(proxy_id, message);
		}
	};
	proxy->on_login += [this, proxy_id](Proxy::ConnectionControl& control) {
		if (WhiteBlackList::IsInBlack(control.Username()))
		{
			control.isEnableConnect = false;
			control.reason = "You are in black list";
			this->EmitLog(proxy_id, "Login denied for " + control.Username() + ": black list");
		}
		else if (WhiteBlackList::IsWhiteListOn() && !WhiteBlackList::IsInWhite(control.Username()))
		{
			control.isEnableConnect = false;
			control.reason = "You are not in white list";
			this->EmitLog(proxy_id, "Login denied for " + control.Username() + ": not in white list");
		}
		else
		{
			this->EmitLog(proxy_id, "Player " + control.Username() + " uuid:" + control.UUID() + " logged in from " + control.GetAddress());
		}
	};
	proxy->on_logout += [this, proxy_id](Proxy::ConnectionControl& control) {
		double flow = control.UploadBytes();
		std::string unit = "bytes";
		if (flow > 10000)
		{
			flow /= 1024;
			unit = "KB";
		}
		if (flow > 10000)
		{
			flow /= 1024;
			unit = "MB";
		}
		if (flow > 10000)
		{
			flow /= 1024;
			unit = "GB";
		}
		double time = std::time(nullptr) - control.ConnectTime();
		std::string time_unit = "seconds";
		if (time > 100)
		{
			time /= 60;
			time_unit = "minutes";
		}
		if (time > 100)
		{
			time /= 60;
			time_unit = "hours";
		}
		this->EmitLog(proxy_id, "Player " + control.Username() + " uuid:" + control.UUID() + " logged out from " + control.GetAddress() + ", online duration " + std::to_string(time) + " " + time_unit + ", traffic used " + std::to_string(flow) + " " + unit);
	};
}

auto ProxyManager::Size() const -> std::size_t
{
	std::shared_lock lock(this->mutex);
	return this->records.size();
}

bool ProxyManager::HasProxy(const std::string& proxy_id) const
{
	std::shared_lock lock(this->mutex);
	const std::string key = proxy_id.empty() ? (this->order.empty() ? std::string{} : this->order.front()) : proxy_id;
	return !key.empty() && this->records.find(key) != this->records.end();
}

auto ProxyManager::GetDefaultProxyId() const -> std::string
{
	std::shared_lock lock(this->mutex);
	if (this->order.empty())
	{
		return {};
	}
	return this->order.front();
}

auto ProxyManager::GetProxy(const std::string& proxy_id) const -> std::shared_ptr<Proxy>
{
	std::shared_lock lock(this->mutex);
	std::string key = proxy_id.empty() ? (this->order.empty() ? std::string{} : this->order.front()) : proxy_id;
	auto it = this->records.find(key);
	if (it == this->records.end())
	{
		return {};
	}
	return it->second.proxy;
}

auto ProxyManager::GetConfig(const std::string& proxy_id) const -> ProxyServiceConfig
{
	std::shared_lock lock(this->mutex);
	std::string key = proxy_id.empty() ? (this->order.empty() ? std::string{} : this->order.front()) : proxy_id;
	auto it = this->records.find(key);
	if (it == this->records.end())
	{
		throw ProxyException("Proxy not found");
	}
	return it->second.config;
}

auto ProxyManager::GetView(const std::string& proxy_id) const -> ProxyServiceView
{
	ProxyServiceConfig config;
	std::shared_ptr<Proxy> proxy;
	bool is_default = false;
	{
		std::shared_lock lock(this->mutex);
		std::string key = proxy_id.empty() ? (this->order.empty() ? std::string{} : this->order.front()) : proxy_id;
		auto it = this->records.find(key);
		if (it == this->records.end())
		{
			throw ProxyException("Proxy not found");
		}
		config = it->second.config;
		proxy = it->second.proxy;
		is_default = !this->order.empty() && this->order.front() == it->second.config.id;
	}
	return BuildView(config, proxy, is_default);
}

auto ProxyManager::ListViews() const -> std::vector<ProxyServiceView>
{
	struct Snapshot
	{
		ProxyServiceConfig config;
		std::shared_ptr<Proxy> proxy;
		bool is_default = false;
	};
	std::vector<Snapshot> snapshots;
	{
		std::shared_lock lock(this->mutex);
		snapshots.reserve(this->order.size());
		for (const auto& id : this->order)
		{
			auto it = this->records.find(id);
			if (it == this->records.end())
			{
				continue;
			}
			snapshots.push_back(Snapshot{ it->second.config, it->second.proxy, !this->order.empty() && this->order.front() == it->second.config.id });
		}
	}
	std::vector<ProxyServiceView> views;
	views.reserve(snapshots.size());
	for (const auto& snapshot : snapshots)
	{
		views.push_back(BuildView(snapshot.config, snapshot.proxy, snapshot.is_default));
	}
	return views;
}

void ProxyManager::SaveUnlocked() const
{
	if (this->store_path.empty())
	{
		return;
	}
	neb::CJsonObject root;
	root.Add("version", "1.0");
	root.Add("default_proxy_id", this->order.empty() ? std::string{} : this->order.front());
	root.AddEmptySubArray("proxies");
	for (const auto& id : this->order)
	{
		auto it = this->records.find(id);
		if (it == this->records.end())
		{
			continue;
		}
		neb::CJsonObject item;
		item.Add("id", it->second.config.id);
		item.Add("name", it->second.config.name);
		item.Add("local_address", it->second.config.local_address);
		item.Add("local_port", static_cast<int>(it->second.config.local_port));
		item.Add("remote_address", it->second.config.remote_address);
		item.Add("remote_port", static_cast<int>(it->second.config.remote_port));
		item.Add("max_player", it->second.config.max_player);
		item.Add("motd_path", it->second.config.motd_path);
		root["proxies"].Add(item);
	}
	WriteTextFile(this->store_path, root.ToFormattedString());
}

void ProxyManager::SaveToDisk() const
{
	std::shared_lock lock(this->mutex);
	this->SaveUnlocked();
}

auto ProxyManager::AddProxy(const ProxyServiceConfig& service, bool save_after_add) -> std::string
{
	ProxyServiceConfig config = service;
	ValidateConfig(config);
	{
		std::unique_lock lock(this->mutex);
		if (config.id.empty())
		{
			config.id = GenerateId(this->records, config);
		}
		config.id = NormalizeId(config.id);
		if (config.id.empty())
		{
			config.id = GenerateId(this->records, config);
		}
		if (this->records.find(config.id) != this->records.end())
		{
			throw ProxyException("Proxy id already exists: " + config.id);
		}
		for (const auto& id : this->order)
		{
			const auto& record = this->records.at(id);
			if (record.config.local_port == config.local_port)
			{
				throw ProxyException("Local port already in use: " + std::to_string(config.local_port));
			}
		}
	}
	config.name = MakeDefaultName(config);
	auto proxy = std::make_shared<Proxy>(config.local_address, config.local_port, config.remote_address, config.remote_port, 1);
	AttachCallbacks(config.id, proxy);
	proxy->Start();
	try
	{
		proxy->SetMotd(Motd::LoadMotdFromFile(config.motd_path));
	}
	catch (const std::exception& ex)
	{
		Logger::LogWarn("Proxy %s MOTD load failed: %s", config.id.c_str(), ex.what());
		proxy->SetMotd(Motd::LoadMotdFromFile(""));
	}
	proxy->SetMaxPlayer(config.max_player);
	try
	{
		Logger::LogInfo("Proxy %s ping latency: %lldms", config.id.c_str(), static_cast<long long>(proxy->PingTest()));
	}
	catch (const std::exception& ex)
	{
		Logger::LogWarn("Proxy %s ping test failed: %s", config.id.c_str(), ex.what());
	}
	{
		std::unique_lock lock(this->mutex);
		if (this->records.find(config.id) != this->records.end())
		{
			throw ProxyException("Proxy id already exists: " + config.id);
		}
		for (const auto& id : this->order)
		{
			const auto& record = this->records.at(id);
			if (record.config.local_port == config.local_port)
			{
				throw ProxyException("Local port already in use: " + std::to_string(config.local_port));
			}
		}
		this->order.push_back(config.id);
		this->records.emplace(config.id, ProxyRecord{ config, proxy });
	}
	if (save_after_add)
	{
		this->SaveToDisk();
	}
	Logger::LogInfo("Created proxy service %s listening at %s:%d -> %s:%d", config.id.c_str(), config.local_address.c_str(), config.local_port, config.remote_address.c_str(), config.remote_port);
	return config.id;
}

bool ProxyManager::RemoveProxy(const std::string& proxy_id, bool save_after_remove)
{
	std::string key;
	ProxyRecord removed_record;
	{
		std::unique_lock lock(this->mutex);
		key = proxy_id.empty() ? (this->order.empty() ? std::string{} : this->order.front()) : proxy_id;
		auto it = this->records.find(key);
		if (it == this->records.end())
		{
			return false;
		}
		removed_record = std::move(it->second);
		this->records.erase(it);
		this->order.erase(std::remove(this->order.begin(), this->order.end(), key), this->order.end());
	}
	if (save_after_remove)
	{
		this->SaveToDisk();
	}
	Logger::LogInfo("Removed proxy service %s", key.c_str());
	return true;
}

void ProxyManager::LoadFromDiskOrCreate(const ProxyServiceConfig& fallback_service)
{
	std::vector<ProxyServiceConfig> loaded_services;
	std::string path = this->GetStorePath();
	if (!path.empty())
	{
		const std::string content = ReadTextFile(path);
		if (!content.empty())
		{
			neb::CJsonObject root;
			if (root.Parse(content))
			{
				neb::CJsonObject proxies;
				if ((root.Get("proxies", proxies) || root.Get("servers", proxies)) && proxies.IsArray())
				{
					for (int index = 0; index < proxies.GetArraySize(); ++index)
					{
						try
						{
							neb::CJsonObject item;
							if (!proxies.Get(index, item))
							{
								continue;
							}
							ProxyServiceConfig service;
							std::string text;
							int number = 0;
							if (item.Get("id", text)) service.id = text;
							if (item.Get("name", text)) service.name = text;
							if (item.Get("local_address", text)) service.local_address = text;
							if (item.Get("remote_address", text)) service.remote_address = text;
							if (item.Get("motd_path", text)) service.motd_path = text;
							if (item.Get("local_port", number)) service.local_port = ClampPort(number, "local_port");
							if (item.Get("remote_port", number)) service.remote_port = ClampPort(number, "remote_port");
							if (item.Get("max_player", number)) service.max_player = number;
							loaded_services.push_back(service);
						}
						catch (const std::exception& ex)
						{
							Logger::LogWarn("Skip invalid proxy record: %s", ex.what());
						}
					}
				}
				else if (root.IsArray())
				{
					for (int index = 0; index < root.GetArraySize(); ++index)
					{
						try
						{
							neb::CJsonObject item;
							if (!root.Get(index, item))
							{
								continue;
							}
							ProxyServiceConfig service;
							std::string text;
							int number = 0;
							if (item.Get("id", text)) service.id = text;
							if (item.Get("name", text)) service.name = text;
							if (item.Get("local_address", text)) service.local_address = text;
							if (item.Get("remote_address", text)) service.remote_address = text;
							if (item.Get("motd_path", text)) service.motd_path = text;
							if (item.Get("local_port", number)) service.local_port = ClampPort(number, "local_port");
							if (item.Get("remote_port", number)) service.remote_port = ClampPort(number, "remote_port");
							if (item.Get("max_player", number)) service.max_player = number;
							loaded_services.push_back(service);
						}
						catch (const std::exception& ex)
						{
							Logger::LogWarn("Skip invalid proxy record: %s", ex.what());
						}
					}
				}
			}
		}
	}
	if (loaded_services.empty())
	{
		loaded_services.push_back(fallback_service);
	}
	for (const auto& service : loaded_services)
	{
		try
		{
			this->AddProxy(service, false);
		}
		catch (const std::exception& ex)
		{
			Logger::LogWarn("Skip proxy service during load: %s", ex.what());
		}
	}
	if (this->Size() == 0)
	{
		this->AddProxy(fallback_service, false);
	}
	this->SaveToDisk();
}

void ProxyManager::SetMaxPlayer(const std::string& proxy_id, int max_player, bool save_after_set)
{
	std::unique_lock lock(this->mutex);
	std::string key = proxy_id.empty() ? (this->order.empty() ? std::string{} : this->order.front()) : proxy_id;
	auto it = this->records.find(key);
	if (it == this->records.end())
	{
		throw ProxyException("Proxy not found");
	}
	it->second.config.max_player = max_player;
	if (it->second.proxy)
	{
		it->second.proxy->SetMaxPlayer(max_player);
	}
	lock.unlock();
	if (save_after_set)
	{
		this->SaveToDisk();
	}
}

void ProxyManager::ReloadMotd(const std::string& proxy_id)
{
	std::shared_ptr<Proxy> proxy;
	ProxyServiceConfig config;
	{
		std::shared_lock lock(this->mutex);
		std::string key = proxy_id.empty() ? (this->order.empty() ? std::string{} : this->order.front()) : proxy_id;
		auto it = this->records.find(key);
		if (it == this->records.end())
		{
			throw ProxyException("Proxy not found");
		}
		proxy = it->second.proxy;
		config = it->second.config;
	}
	if (!proxy)
	{
		throw ProxyException("Proxy is not running");
	}
	proxy->SetMotd(Motd::LoadMotdFromFile(config.motd_path));
}

auto ProxyManager::GetDefaultProxy() const -> std::shared_ptr<Proxy>
{
	return this->GetProxy();
}
