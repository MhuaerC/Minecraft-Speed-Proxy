#pragma once

#include "proxy.h"
#include <functional>
#include <ctime>
#include <map>
#include <memory>
#include <shared_mutex>
#include <string>
#include <vector>

struct ProxyServiceConfig
{
	std::string id;
	std::string name;
	std::string local_address = "0.0.0.0";
	std::uint16_t local_port = 25565;
	std::string remote_address = "mc.hypixel.net";
	std::uint16_t remote_port = 25565;
	int max_player = -1;
	std::string motd_path;
};

struct ProxyServiceView
{
	std::string id;
	std::string name;
	std::string local_address;
	std::uint16_t local_port = 0;
	std::string remote_address;
	std::uint16_t remote_port = 0;
	int max_player = -1;
	std::string motd_path;
	bool running = false;
	bool is_default = false;
	std::time_t start_time = 0;
	std::uint32_t online_users = 0;
	std::string default_proxy;
};

class ProxyManager
{
public:
	using LogSink = std::function<void(const std::string& proxy_id, const std::string& message)>;

	ProxyManager() = default;
	~ProxyManager() noexcept;

	void SetStorePath(std::string path);
	auto GetStorePath() const -> std::string;

	void SetLogSink(LogSink sink);

	void LoadFromDiskOrCreate(const ProxyServiceConfig& fallback_service);
	void SaveToDisk() const;

	auto AddProxy(const ProxyServiceConfig& service, bool save_after_add = true) -> std::string;
	bool RemoveProxy(const std::string& proxy_id, bool save_after_remove = true);
	bool HasProxy(const std::string& proxy_id) const;

	auto GetProxy(const std::string& proxy_id = "") const -> std::shared_ptr<Proxy>;
	auto GetConfig(const std::string& proxy_id = "") const -> ProxyServiceConfig;
	auto GetView(const std::string& proxy_id = "") const -> ProxyServiceView;
	auto ListViews() const -> std::vector<ProxyServiceView>;
	auto GetDefaultProxyId() const -> std::string;
	auto GetDefaultProxy() const -> std::shared_ptr<Proxy>;

	void SetMaxPlayer(const std::string& proxy_id, int max_player, bool save_after_set = true);
	void ReloadMotd(const std::string& proxy_id);

	auto Size() const -> std::size_t;

private:
	struct ProxyRecord
	{
		ProxyServiceConfig config;
		std::shared_ptr<Proxy> proxy;
	};

	mutable std::shared_mutex mutex;
	std::map<std::string, ProxyRecord> records;
	std::vector<std::string> order;
	std::string store_path;
	LogSink log_sink;

	static void ValidateConfig(const ProxyServiceConfig& service);
	static auto NormalizeId(std::string id) -> std::string;
	static auto MakeDefaultName(const ProxyServiceConfig& service) -> std::string;
	static auto GenerateId(const std::map<std::string, ProxyRecord>& records, const ProxyServiceConfig& service) -> std::string;
	void AttachCallbacks(const std::string& proxy_id, const std::shared_ptr<Proxy>& proxy);
	void EmitLog(const std::string& proxy_id, const std::string& message) const;
	void SaveUnlocked() const;
};
