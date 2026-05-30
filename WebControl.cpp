#include "WebControl.h"
#include "webpanel_assets.h"
#include "logger.h"
#include <regex>
#include "json/CJsonObject.h"
#include "rbslib/CharsetConvert.h"
#include "WhiteBlackList.h"
#include "config.h"
#include <cmath>
#include <map>
#include <mutex>
#include <ranges>
#include <sstream>

namespace
{
	auto PathOnly(const std::string& raw_path) -> std::string
	{
		std::string path = raw_path;
		if (auto pos = path.find('?'); pos != std::string::npos)
		{
			path.resize(pos);
		}
		return path;
	}

	auto QueryValue(const std::string& raw_path, const std::string& key) -> std::string
	{
		auto query_pos = raw_path.find('?');
		if (query_pos == std::string::npos)
		{
			return {};
		}
		std::string query = raw_path.substr(query_pos + 1);
		std::stringstream stream(query);
		std::string part;
		while (std::getline(stream, part, '&'))
		{
			auto equal_pos = part.find('=');
			if (equal_pos == std::string::npos)
			{
				continue;
			}
			if (part.substr(0, equal_pos) == key)
			{
				return part.substr(equal_pos + 1);
			}
		}
		return {};
	}

	auto RequestProxyId(const std::string& raw_path, const neb::CJsonObject* request, const std::shared_ptr<ProxyManager>& proxy_manager) -> std::string
	{
		std::string proxy_id = QueryValue(raw_path, "proxy_id");
		if (proxy_id.empty() && request)
		{
			request->Get("proxy_id", proxy_id);
		}
		if (proxy_id.empty() && proxy_manager)
		{
			proxy_id = proxy_manager->GetDefaultProxyId();
		}
		return proxy_id;
	}

	void AddProxyViewJson(neb::CJsonObject& object, const ProxyServiceView& view)
	{
		object.Add("id", view.id);
		object.Add("name", view.name);
		object.Add("local_address", view.local_address);
		object.Add("local_port", static_cast<int>(view.local_port));
		object.Add("remote_address", view.remote_address);
		object.Add("remote_port", static_cast<int>(view.remote_port));
		object.Add("max_player", view.max_player);
		object.Add("motd_path", view.motd_path);
		object.Add("running", view.running, view.running);
		object.Add("is_default", view.is_default, view.is_default);
		object.Add("start_time", static_cast<int64_t>(view.start_time));
		object.Add("online_users", static_cast<int>(view.online_users));
		object.Add("default_proxy", view.default_proxy);
		object.Add("listen_endpoint", view.local_address + ":" + std::to_string(view.local_port));
	}
}

asio::awaitable<void> WebControlServer::TimeTaskUsers(std::shared_ptr<ProxyManager> proxy_manager)
{
	try
	{
		asio::steady_timer timer(co_await asio::this_coro::executor);
		while (true) 
		{
			auto views = proxy_manager ? proxy_manager->ListViews() : std::vector<ProxyServiceView>{};
			auto now = std::time(nullptr);
			std::unique_lock<std::shared_mutex> lock(time_online_users_mutex);
			for (const auto& view : views)
			{
				time_online_users[view.id][now] = view.online_users;
			}
			lock.unlock();
			timer.expires_after(std::chrono::minutes(1));
			co_await timer.async_wait(asio::use_awaitable);
		}
	}
	catch (...){}
}

void WebControlServer::SendTextResponse(const RbsLib::Network::TCP::TCPConnection& connection, const std::string& body, const std::string& content_type, int http_status_code)
{
	RbsLib::Network::HTTP::ResponseHeader response;
	response.status = http_status_code;
	response.headers.AddHeader("Content-Type", content_type);
	response.headers.AddHeader("Content-Length", std::to_string(body.size()));
	response.headers.AddHeader("Access-Control-Allow-Origin", "*");
	response.headers.AddHeader("Cache-Control", "no-store, max-age=0");
	connection.Send(response.ToBuffer());
	if (!body.empty())
	{
		connection.Send(body.c_str(), body.size());
	}
}

void WebControlServer::SendErrorResponse(const RbsLib::Network::TCP::TCPConnection& connection, int status_code, const std::string& message)
{
	neb::CJsonObject json;
	json.Add("status", status_code);
	json.Add("message", RbsLib::Encoding::CharsetConvert::ANSItoUTF8(message));
	SendErrorResponse(connection, json);
}

void WebControlServer::SendErrorResponse(const RbsLib::Network::TCP::TCPConnection& connection, const neb::CJsonObject& json, int http_status_code)
{
	RbsLib::Network::HTTP::ResponseHeader response;
	response.status = http_status_code;
	response.headers.AddHeader("Content-Type", "application/json; charset=utf-8");
	response.headers.AddHeader("Content-Length", std::to_string(json.ToString().size()));
	response.headers.AddHeader("Access-Control-Allow-Origin", "*");
	RbsLib::Buffer response_buffer = response.ToBuffer();
	connection.Send(response.ToBuffer());
	connection.Send(json.ToString().c_str(), json.ToString().size());
}

void WebControlServer::SendSuccessResponse(const RbsLib::Network::TCP::TCPConnection& connection, const neb::CJsonObject& json)
{
	neb::CJsonObject wait_send_json = json;
	wait_send_json.Add("status", 200);
	wait_send_json.Add("message", "OK");
	auto json_str = wait_send_json.ToString();
	RbsLib::Network::HTTP::ResponseHeader response;
	response.status = 200;
	response.headers.AddHeader("Content-Type", "application/json; charset=utf-8");
	response.headers.AddHeader("Content-Length", std::to_string(json_str.size()));
	response.headers.AddHeader("Access-Control-Allow-Origin", "*");
	connection.Send(response.ToBuffer());
	connection.Send(json_str.c_str(), json_str.length());
}

bool WebControlServer::CheckToken(const std::string& cookie, const std::string& token, const std::chrono::system_clock::time_point& token_expiry_time)
{

	std::string extracted_token = cookie;
	return (extracted_token == token) && (std::chrono::system_clock::now() < token_expiry_time);
}

void WebControlServer::GetStatus(neb::CJsonObject& response, const ProxyServiceView& view, const std::shared_ptr<Proxy>& proxy_client)
{
	auto start_time = proxy_client->GetStartTime();
	auto now_time = std::time(nullptr);
	auto default_proxy = proxy_client->GetDefaultProxy();
	bool whitelist_status = WhiteBlackList::IsWhiteListOn();

	response.Add("proxy_id", view.id);
	response.Add("proxy_name", view.name);
	response.Add("local_address", view.local_address);
	response.Add("local_port", static_cast<int>(view.local_port));
	response.Add("remote_address", view.remote_address);
	response.Add("remote_port", static_cast<int>(view.remote_port));
	response.Add("listen_endpoint", view.local_address + ":" + std::to_string(view.local_port));
	response.Add("start_time", start_time);
	response.Add("now_time", now_time);
	response.Add("uptime_seconds", start_time > 0 && now_time > start_time ? static_cast<int>(now_time - start_time) : 0);
	response.Add("online_users", static_cast<int>(proxy_client->GetUsersInfo().size()));
	response.Add("max_player", proxy_client->GetMaxPlayer());
	response.Add("whitelist_status", whitelist_status, whitelist_status);
	response.Add("default_proxy", default_proxy.first + ":" + std::to_string(default_proxy.second));
}

void WebControlServer::GetOnlineUsers(neb::CJsonObject& response, const std::shared_ptr<Proxy>& proxy_client)
{
	auto online_user_list = proxy_client->GetUsersInfo();
	auto proxy_map = proxy_client->GetUserProxyMap();
	auto default_proxy = proxy_client->GetDefaultProxy();
	response.AddEmptySubArray("online_users");
	for (const auto& user : online_user_list)
	{
		neb::CJsonObject user_info;
		user_info.Add("username", user.username);
		user_info.Add("uuid", user.uuid);
		user_info.Add("ip", user.ip);
		user_info.Add("current_proxy_flow", user.upload_bytes);
		user_info.Add("total_proxy_flow", 0);
		user_info.Add("online_time_stamp", user.connect_time);
		if (auto proxy_item = proxy_map.find(user.username); proxy_item != proxy_map.end())
		{
			user_info.Add("proxy_target", proxy_item->second.first + ":" + std::to_string(proxy_item->second.second));
		}
		else
		{
			user_info.Add("proxy_target", default_proxy.first + ":" + std::to_string(default_proxy.second));
		}
		response["online_users"].Add(user_info);
	}
}

void WebControlServer::GetWhiteList(neb::CJsonObject& response, const std::shared_ptr<Proxy>& proxy_client)
{
	auto white_list = WhiteBlackList::GetWhiteList();
	bool is_whitelist_on = WhiteBlackList::IsWhiteListOn();
	response.Add("whitelist_status", is_whitelist_on,is_whitelist_on);
	response.AddEmptySubArray("white_list");
	for (const auto& user : white_list)
	{
		response["white_list"].Add(user);
	}
}

void WebControlServer::GetBlackList(neb::CJsonObject& response, const std::shared_ptr<Proxy>& proxy_client)
{
	auto black_list = WhiteBlackList::GetBlackList();
	response.AddEmptySubArray("black_list");
	for (const auto& user : black_list)
	{
		response["black_list"].Add(user);
	}
}

bool WebControlServer::AddBlacklistUser(neb::CJsonObject& response, const neb::CJsonObject& request, const std::shared_ptr<Proxy>& proxy_client)
{
	std::string username;
	if (!request.Get("username",username))
		{
		response.Add("status", 400);
		response.Add("message", "Missing 'username' field in request");
		return false;
	}
	if (WhiteBlackList::IsInBlack(username))
	{
		response.Add("status", 400);
		response.Add("message", "User is already in black list");
		return false;
	}
	WhiteBlackList::AddBlackList(username);
	Logger::LogInfo("WebPanel: Added user %s to blacklist", username.c_str());
	response.Add("status", 200);
	response.Add("message", "User added to black list successfully");
	return true;
}

bool WebControlServer::RemoveBlacklistUser(neb::CJsonObject& response, const neb::CJsonObject& request, const std::shared_ptr<Proxy>& proxy_client)
{
	std::string username;
	if (!request.Get("username", username))
	{
		response.Add("status", 400);
		response.Add("message", "Missing 'username' field in request");
		return false;
	}
	if (!WhiteBlackList::IsInBlack(username))
	{
		response.Add("status", 400);
		response.Add("message", "User is not in black list");
		return false;
	}
	WhiteBlackList::RemoveBlackList(username);
	Logger::LogInfo("WebPanel: Removed user %s from blacklist", username.c_str());
	response.Add("status", 200);
	response.Add("message", "User removed from black list successfully");
	return true;
}

bool WebControlServer::AddWhitelistUser(neb::CJsonObject& response, const neb::CJsonObject& request, const std::shared_ptr<Proxy>& proxy_client)
{
	std::string username;
	if (!request.Get("username", username))
	{
		response.Add("status", 400);
		response.Add("message", "Missing 'username' field in request");
		return false;
	}
	if (WhiteBlackList::IsInWhite(username))
	{
		response.Add("status", 400);
		response.Add("message", "User is already in white list");
		return false;
	}
	WhiteBlackList::AddWhiteList(username);
	Logger::LogInfo("WebPanel: Added user %s to whitelist", username.c_str());
	response.Add("status", 200);
	response.Add("message", "User added to white list successfully");
	return true;
}

bool WebControlServer::RemoveWhitelistUser(neb::CJsonObject& response, const neb::CJsonObject& request, const std::shared_ptr<Proxy>& proxy_client)
{
	std::string username;
	if (!request.Get("username", username))
	{
		response.Add("status", 400);
		response.Add("message", "Missing 'username' field in request");
		return false;
	}
	if (!WhiteBlackList::IsInWhite(username))
	{
		response.Add("status", 400);
		response.Add("message", "User is not in white list");
		return false;
	}
	WhiteBlackList::RemoveWhiteList(username);
	Logger::LogInfo("WebPanel: Removed user %s from whitelist", username.c_str());
	response.Add("status", 200);
	response.Add("message", "User removed from white list successfully");
	return true;
}

bool WebControlServer::EnableWhitelist(neb::CJsonObject& response, const std::shared_ptr<Proxy>& proxy_client)
{
	if (WhiteBlackList::IsWhiteListOn())
	{
		response.Add("status", 400);
		response.Add("message", "Whitelist is already enabled");
		return false;
	}
	else
	{
		WhiteBlackList::WhiteListOn();
		Logger::LogInfo("WebPanel: Enabled whitelist");
		response.Add("status", 200);
		response.Add("message", "Whitelist enabled successfully");
		return true;
	}
}

bool WebControlServer::DisableWhitelist(neb::CJsonObject& response, const std::shared_ptr<Proxy>& proxy_client)
{
	if (!WhiteBlackList::IsWhiteListOn())
	{
		response.Add("status", 400);
		response.Add("message", "Whitelist is already disabled");
		return false;
	}
	else
	{
		WhiteBlackList::WhiteListOff();
		Logger::LogInfo("WebPanel: Disabled whitelist");
		response.Add("status", 200);
		response.Add("message", "Whitelist disabled successfully");
		return true;
	}
}

void WebControlServer::GetUserProxyList(neb::CJsonObject& response, const std::shared_ptr<Proxy>& proxy_client)
{
	auto proxy_map = proxy_client->GetUserProxyMap();
	auto default_proxy = proxy_client->GetDefaultProxy();
	response.Add("default_proxy", default_proxy.first + ":" + std::to_string(default_proxy.second));
	response.AddEmptySubArray("user_proxies");
	for (const auto& item : proxy_map)
	{
		neb::CJsonObject user_proxy;
		user_proxy.Add("username", item.first);
		user_proxy.Add("proxy_target_addr", item.second.first);
		user_proxy.Add("proxy_target_port", item.second.second);
		response["user_proxies"].Add(user_proxy);
	}
}

bool WebControlServer::SetUserProxy(neb::CJsonObject& response, const neb::CJsonObject& request, const std::shared_ptr<Proxy>& proxy_client)
{
	std::string username;
	std::string proxy_address;
	int proxy_port;
	if (!request.Get("username", username) || !request.Get("proxy_address", proxy_address) || !request.Get("proxy_port", proxy_port))
	{
		response.Add("status", 400);
		response.Add("message", "Missing 'username', 'proxy_address' or 'proxy_port' field in request");
		return false;
	}
	if (proxy_port < 1 || proxy_port > 65535)
	{
		response.Add("status", 400);
		response.Add("message", "Invalid 'proxy_port' value, must be in range 1-65535");
		return false;
	}
	proxy_client->SetUserProxy(username, proxy_address, static_cast<std::uint16_t>(proxy_port));
	Logger::LogInfo("WebPanel: Set proxy server for user %s to %s:%d", username.c_str(), proxy_address.c_str(), proxy_port);
	response.Add("status", 200);
	response.Add("message", "User proxy set successfully");
	return true;
}

bool WebControlServer::RemoveUserProxy(neb::CJsonObject& response, const neb::CJsonObject& request, const std::shared_ptr<Proxy>& proxy_client)
{
	std::string username;
	if (!request.Get("username", username))
	{
		response.Add("status", 400);
		response.Add("message", "Missing 'username' field in request");
		return false;
	}
	try
	{
		proxy_client->DeleteUserProxy(username);
		Logger::LogInfo("WebPanel: Deleted proxy server setting for user %s", username.c_str());
	}
	catch (ProxyException const& ex)
	{
		response.Add("status", 400);
		response.Add("message", ex.what());
		return false;
	}
	response.Add("status", 200);
	response.Add("message", "User proxy removed successfully");
	return true;
}

bool WebControlServer::SetMaxUsers(neb::CJsonObject& response, const neb::CJsonObject& request, const std::shared_ptr<ProxyManager>& proxy_manager, const std::string& proxy_id)
{
	if (!proxy_manager)
	{
		response.Add("status", 500);
		response.Add("message", "Proxy manager is not available");
		return false;
	}
	int max_users;
	if (!request.Get("max_users", max_users) || max_users < -1)
	{
		response.Add("status", 400);
		response.Add("message", "Invalid 'max_users' value");
		return false;
	}
	try
	{
		proxy_manager->SetMaxPlayer(proxy_id, max_users);
	}
	catch (const std::exception& ex)
	{
		response.Add("status", 404);
		response.Add("message", ex.what());
		return false;
	}
	Logger::LogInfo("WebPanel: Set maximum users to %d", max_users);
	response.Add("status", 200);
	response.Add("message", "Max users set successfully");
	return true;
}

bool WebControlServer::KickPlayer(neb::CJsonObject& response, const neb::CJsonObject& request, const std::shared_ptr<Proxy>& proxy_client)
{
	std::string username;
	if (!request.Get("username", username))
	{
		response.Add("status", 400);
		response.Add("message", "Missing 'username' field in request");
		return false;
	}
	try
	{
		proxy_client->KickByUsername(username);
		Logger::LogInfo("WebPanel: Kicked user %s", username.c_str());
		response.Add("status", 200);
		response.Add("message", "Player kicked successfully");
		return true;
	}
	catch (ProxyException const& ex)
	{
		response.Add("status", 400);
		response.Add("message", ex.what());
		return false;
	}
}

void WebControlServer::GetStartTime(neb::CJsonObject& response, const std::shared_ptr<Proxy>& proxy_client)
{
	auto start_time = proxy_client->GetStartTime();
	response.Add("start_time", start_time);
	response.Add("now_time", std::time(nullptr));
	response.Add("status", 200);
	response.Add("message", "Start time retrieved successfully");
}

// �޸ĵĺ��� GetUserNumberList���ص�����
bool WebControlServer::GetUserNumberList(neb::CJsonObject& response, neb::CJsonObject& request, const std::string& proxy_id)
{
	// ��ȡʱ�䷶Χ
	std::time_t start_time, end_time;
	if (!request.Get("start_time", start_time) || !request.Get("end_time", end_time) || start_time >= end_time)
	{
		response.Add("status", 400);
		response.Add("message", "Invalid 'start_time' or 'end_time' value");
		return false;
	}
	// ��ȡ���� minute�����ӣ���hour��Сʱ����day���죩��week���ܣ���month���£�
	std::string granularity;
	if (!request.Get("granularity", granularity) || (granularity != "minute" && granularity != "hour" && granularity != "day" && granularity != "week" && granularity != "month"))
	{
		response.Add("status", 400);
		response.Add("message", "Invalid 'granularity' value");
		return false;
	}
	std::map<std::time_t, uint32_t> history;
	{
		std::shared_lock<std::shared_mutex> lock(this->time_online_users_mutex);
		if (auto item = this->time_online_users.find(proxy_id); item != this->time_online_users.end())
		{
			history = item->second;
		}
	}

	// �������ȷ���ͳ��
	std::map<std::time_t, uint32_t> grouped,count;
	for (const auto& item : history)
	{
		if (item.first < start_time || item.first > end_time)
		{
			continue;
		}
		std::time_t key = 0;
		if (granularity == "minute")
		{
			key = item.first / 60 * 60;
		}
		else if (granularity == "hour")
		{
			key = item.first / 3600 * 3600;
		}
		else if (granularity == "day")
		{
			key = item.first / 86400 * 86400;
		}
		else if (granularity == "week")
		{
			key = item.first / (7 * 86400) * (7 * 86400);
		}
		else if (granularity == "month")
		{
			std::tm tm = *std::localtime(&item.first);
			tm.tm_mday = 1; // ����Ϊ���µ�һ��
			tm.tm_hour = 0;
			tm.tm_min = 0;
			tm.tm_sec = 0;
			key = std::mktime(&tm);
		}
		grouped[key] += item.second;
		count[key]++; // ͳ��ÿ��ʱ��ε����ݸ���
	}
	// ����ÿ��ʱ���ƽ�������û���
	for (auto& item : grouped)
	{
		if (count[item.first] > 0)
		{
			item.second = static_cast<int>(std::round(static_cast<double>(item.second) / count[item.first])); // ����ƽ��ֵ
		}
		else
		{
			item.second = 0; // ���û������������Ϊ0
		}
	}

	response.AddEmptySubArray("user_numbers");
	for (const auto& item : grouped)
	{
		neb::CJsonObject user_number;
		user_number.Add("timestamp", item.first);
		user_number.Add("online_users", item.second);
		response["user_numbers"].Add(user_number);
	}
	response.Add("status", 200);
	response.Add("message", "User number list retrieved successfully");
	return true;
}

void WebControlServer::GetLogs(neb::CJsonObject& response, const std::string& proxy_id)
{
	response.AddEmptySubArray("logs");
	std::shared_lock<std::shared_mutex> lock(this->log_mutex);
	auto item = this->logs.find(proxy_id);
	if (item == this->logs.end())
	{
		response.Add("status", 200);
		response.Add("message", "Logs retrieved successfully");
		return;
	}
	for (const auto& log : item->second)
	{
		neb::CJsonObject log_entry;
		log_entry.Add("timestamp", log.first);
		log_entry.Add("message", log.second);
		response["logs"].Add(log_entry);
	}
	response.Add("status", 200);
	response.Add("message", "Logs retrieved successfully");
}

void WebControlServer::GetMotd(neb::CJsonObject& response, const std::shared_ptr<Proxy>& proxy_client)
{
	response.Add("motd", proxy_client->GetMotd());
	response.Add("status", 200);
	response.Add("message", "MOTD retrieved successfully");
}

bool WebControlServer::SetMotd(neb::CJsonObject& response, const neb::CJsonObject& request, const std::shared_ptr<Proxy>& proxy_client)
{
	neb::CJsonObject motd;
	if (request.Get("motd", motd))
	{
		proxy_client->SetMotd(motd.ToString());
		Logger::LogInfo("WebPanel: Set new MOTD");
		response.Add("status", 200);
		response.Add("message", "MOTD set successfully");
		return true;
	}
	else
	{
		response.Add("status", 400);
		response.Add("message", "Missing or invalid 'motd' field in request");
		return false;
	}
}

bool WebControlServer::ReloadMotd(neb::CJsonObject& response, const std::shared_ptr<ProxyManager>& proxy_manager, const std::string& proxy_id)
{
	if (!proxy_manager)
	{
		response.Add("status", 500);
		response.Add("message", "Proxy manager is not available");
		return false;
	}
	try
	{
		proxy_manager->ReloadMotd(proxy_id);
		Logger::LogInfo("WebPanel: Reloaded MOTD from config file");
		response.Add("status", 200);
		response.Add("message", "MOTD reloaded successfully");
		return true;
	}
	catch (const std::exception& ex)
	{
		response.Add("status", 400);
		response.Add("message", ex.what());
		return false;
	}
}

void WebControlServer::GetProxyServers(neb::CJsonObject& response, const std::shared_ptr<ProxyManager>& proxy_manager)
{
	response.Add("default_proxy_id", proxy_manager ? proxy_manager->GetDefaultProxyId() : std::string{});
	response.AddEmptySubArray("proxies");
	if (!proxy_manager)
	{
		return;
	}
	for (const auto& view : proxy_manager->ListViews())
	{
		neb::CJsonObject item;
		AddProxyViewJson(item, view);
		response["proxies"].Add(item);
	}
}

bool WebControlServer::CreateProxyServer(neb::CJsonObject& response, const neb::CJsonObject& request, const std::shared_ptr<ProxyManager>& proxy_manager)
{
	if (!proxy_manager)
	{
		response.Add("status", 500);
		response.Add("message", "Proxy manager is not available");
		return false;
	}
	ProxyServiceConfig service;
	std::string text;
	int number = 0;
	if (request.Get("id", text)) service.id = text;
	if (request.Get("name", text)) service.name = text;
	if (request.Get("local_address", text)) service.local_address = text;
	if (request.Get("remote_address", text)) service.remote_address = text;
	if (request.Get("motd_path", text)) service.motd_path = text;
	if (!request.Get("local_port", number) || number < 1 || number > 65535)
	{
		response.Add("status", 400);
		response.Add("message", "Invalid 'local_port' value");
		return false;
	}
	service.local_port = static_cast<std::uint16_t>(number);
	if (!request.Get("remote_port", number) || number < 1 || number > 65535)
	{
		response.Add("status", 400);
		response.Add("message", "Invalid 'remote_port' value");
		return false;
	}
	service.remote_port = static_cast<std::uint16_t>(number);
	if (request.Get("max_player", number))
	{
		if (number < -1)
		{
			response.Add("status", 400);
			response.Add("message", "Invalid 'max_player' value");
			return false;
		}
		service.max_player = number;
	}
	if (service.remote_address.empty())
	{
		response.Add("status", 400);
		response.Add("message", "Missing 'remote_address' field");
		return false;
	}
	try
	{
		auto proxy_id = proxy_manager->AddProxy(service);
		response.Add("proxy_id", proxy_id);
		AddProxyViewJson(response, proxy_manager->GetView(proxy_id));
		return true;
	}
	catch (const std::exception& ex)
	{
		response.Add("status", 400);
		response.Add("message", ex.what());
		return false;
	}
}

bool WebControlServer::RemoveProxyServer(neb::CJsonObject& response, const neb::CJsonObject& request, const std::shared_ptr<ProxyManager>& proxy_manager)
{
	if (!proxy_manager)
	{
		response.Add("status", 500);
		response.Add("message", "Proxy manager is not available");
		return false;
	}
	std::string proxy_id;
	if (!request.Get("proxy_id", proxy_id) || proxy_id.empty())
	{
		response.Add("status", 400);
		response.Add("message", "Missing 'proxy_id' field");
		return false;
	}
	if (!proxy_manager->RemoveProxy(proxy_id))
	{
		response.Add("status", 404);
		response.Add("message", "Proxy not found");
		return false;
	}
	{
		std::unique_lock<std::shared_mutex> lock(this->log_mutex);
		this->logs.erase(proxy_id);
	}
	{
		std::unique_lock<std::shared_mutex> lock(this->time_online_users_mutex);
		this->time_online_users.erase(proxy_id);
	}
	response.Add("proxy_id", proxy_id);
	return true;
}


WebControlServer::WebControlServer(const std::string& address, std::uint16_t port)
	:server(address, port)
{
}

WebControlServer::~WebControlServer() noexcept
{
	if (!this->is_request_stop)
	{
		this->Stop();
	}
}

void WebControlServer::SetUserPassword(const std::string& password)
{
	this->user_password = std::make_shared<std::string>(password);
}

void WebControlServer::AppendProxyLog(const std::string& proxy_id, const std::string& message)
{
	std::unique_lock<std::shared_mutex> lock(this->log_mutex);
	auto& list = this->logs[proxy_id];
	list.push_back({ std::time(nullptr), RbsLib::Encoding::CharsetConvert::ANSItoUTF8(message) });
	while (list.size() > static_cast<std::size_t>(this->max_log_size))
	{
		list.pop_front();
	}
}

void WebControlServer::Start(std::shared_ptr<ProxyManager>& proxy_manager)
{
	this->proxy_manager = proxy_manager;
	this->server.SetGetHandle([proxy_manager = proxy_manager, this](const RbsLib::Network::TCP::TCPConnection& connection, RbsLib::Network::HTTP::RequestHeader& header) -> int {
		static const std::regex re_userproxy(R"(^/api/([a-zA-Z0-9_]{1,256})$)");
		std::string path = PathOnly(header.path);
		if (path == "/" || path == "/panel" || path == "/index.html")
		{
			WebControlServer::SendTextResponse(connection, std::string(kWebPanelHtml), "text/html; charset=utf-8");
			return 0;
		}
		if (path == "/panel.css")
		{
			WebControlServer::SendTextResponse(connection, std::string(kWebPanelCss), "text/css; charset=utf-8");
			return 0;
		}
		if (path == "/panel.js")
		{
			WebControlServer::SendTextResponse(connection, GetWebPanelJs(), "application/javascript; charset=utf-8");
			return 0;
		}
		if (path == "/favicon.ico")
		{
			WebControlServer::SendTextResponse(connection, "", "image/x-icon", 204);
			return 0;
		}
		std::cmatch m;
		if (std::regex_match(path.c_str(), m, re_userproxy) and m.size() == 2)
		{
			//���token
			std::string cookie = header.headers.GetHeader("Authorize");
			if (!CheckToken(cookie, this->user_token, this->token_expiry_time))
			{
				WebControlServer::SendErrorResponse(connection, 401, "Unauthorized");
			}
			else
			{
				//����ҵ���߼������ؽ��
				
				neb::CJsonObject response_body;
				const std::string proxy_id = RequestProxyId(header.path, nullptr, proxy_manager);
				auto proxy = proxy_manager ? proxy_manager->GetProxy(proxy_id) : nullptr;
				auto require_proxy = [&]() -> bool {
					if (proxy)
					{
						return true;
					}
					response_body.Add("status", 404);
					response_body.Add("message", "Proxy not found");
					this->SendErrorResponse(connection, response_body, 404);
					return false;
					};
				if (m[1].str() == "logout")
				{
					this->user_token = "";
					this->token_expiry_time = std::chrono::system_clock::time_point();
					Logger::LogInfo("WebPanel: User logged out");
					response_body.Add("status", 200);
					response_body.Add("message", "Logout successful");
					RbsLib::Network::HTTP::ResponseHeader response_header;
					response_header.headers.AddHeader("Access-Control-Allow-Origin", "*");
					response_header.status = 200;
					response_header.headers.AddHeader("Content-Type", "application/json; charset=utf-8");
					response_header.headers.AddHeader("Content-Length", std::to_string(response_body.ToString().size()));
					connection.Send(response_header.ToBuffer());
					auto str = response_body.ToString();
					connection.Send(str.c_str(), str.size());
				}
				else if (m[1].str() == "get_proxy_servers")
				{
					this->GetProxyServers(response_body, proxy_manager);
					this->SendSuccessResponse(connection, response_body);
				}
				else if (m[1].str() == "get_status")
				{
					if (!require_proxy()) return 0;
					try
					{
						this->GetStatus(response_body, proxy_manager->GetView(proxy_id), proxy);
					}
					catch (const std::exception&)
					{
						WebControlServer::SendErrorResponse(connection, 404, "Proxy not found");
						return 0;
					}
					this->SendSuccessResponse(connection, response_body);
				}
				else if (m[1].str() == "get_online_users")
				{
					if (!require_proxy()) return 0;
					this->GetOnlineUsers(response_body, proxy);
					this->SendSuccessResponse(connection, response_body);
				}
				else if (m[1].str() == "get_whitelist")
				{
					this->GetWhiteList(response_body, proxy);
					this->SendSuccessResponse(connection, response_body);
				}
				else if (m[1].str() == "get_blacklist")
				{
					this->GetBlackList(response_body, proxy);
					this->SendSuccessResponse(connection, response_body);
				}
				else if (m[1].str() == "enable_whitelist")
				{
					if (this->EnableWhitelist(response_body, proxy))
					{
						this->SendSuccessResponse(connection, response_body);
					}
					else
					{
						this->SendErrorResponse(connection, response_body);
					}
				}
				else if (m[1].str() == "disable_whitelist")
				{
					if (this->DisableWhitelist(response_body, proxy))
					{
						this->SendSuccessResponse(connection, response_body);
					}
					else
					{
						this->SendErrorResponse(connection, response_body);
					}
				}
				else if (m[1].str() == "get_user_proxies")
				{
					if (!require_proxy()) return 0;
					this->GetUserProxyList(response_body, proxy);
					this->SendSuccessResponse(connection, response_body);
				}
				else if (m[1].str() == "get_start_time")
				{
					if (!require_proxy()) return 0;
					this->GetStartTime(response_body, proxy);
					this->SendSuccessResponse(connection, response_body);
				}
				else if (m[1].str() == "get_logs")
				{
					this->GetLogs(response_body, proxy_id);
					this->SendSuccessResponse(connection, response_body);
				}
				else if (m[1].str() == "get_motd")
				{
					if (!require_proxy()) return 0;
					this->GetMotd(response_body, proxy);
					this->SendSuccessResponse(connection, response_body);
				}
				else
				{
					WebControlServer::SendErrorResponse(connection, 404, "Unknown API endpoint");
				}
			}
		}
		else
		{
			WebControlServer::SendErrorResponse(connection, 404, "Unknown API endpoint");
		}
		return 0;
		});
	this->server.SetPostHandle([proxy_manager = proxy_manager, this](const RbsLib::Network::TCP::TCPConnection& connection, RbsLib::Network::HTTP::RequestHeader& header, RbsLib::Buffer& buffer) -> int {
		static const std::regex re_userproxy(R"(^/api/([a-zA-Z0-9_]{1,256})$)");
		std::string path = PathOnly(header.path);
		std::cmatch m;
		if (std::regex_match(path.c_str(), m, re_userproxy) and m.size() == 2)
		{
			//����JSON
			neb::CJsonObject data;
			if (!data.Parse(buffer.ToString()))
			{
				WebControlServer::SendErrorResponse(connection, 400, "Invalid JSON data");
				return 0;
			}
			if (m[1].str() == "login")
			{
				//��¼�߼�
				std::string password = data("password");
				if (!this->user_password || *this->user_password != password)
				{
					WebControlServer::SendErrorResponse(connection, 401, "Invalid password");
					return 0;
				}
				//����token
				this->user_token = std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
				this->token_expiry_time = std::chrono::system_clock::now() + std::chrono::hours(1); // token��Ч��1Сʱ
				Logger::LogInfo("WebPanel: User successfully logged in via web panel");
				neb::CJsonObject response;
				response.Add("status", 200);
				response.Add("message", "Login successful");
				response.Add("token", this->user_token);
				response.Add("token_expiry_time", std::chrono::system_clock::to_time_t(this->token_expiry_time));
				RbsLib::Network::HTTP::ResponseHeader response_header;
				response_header.status = 200;
				response_header.headers.AddHeader("Content-Type", "application/json; charset=utf-8");
				response_header.headers.AddHeader("Access-Control-Allow-Origin", "*");
				response_header.headers.AddHeader("Content-Length", std::to_string(response.ToString().size()));
				connection.Send(response_header.ToBuffer());
				connection.Send(response.ToString().c_str(), response.ToString().size());
			}
			else
			{
				//���token
				std::string cookie = header.headers.GetHeader("Authorize");
				if (!CheckToken(cookie, this->user_token, this->token_expiry_time))
				{
					WebControlServer::SendErrorResponse(connection, 401, "Unauthorized");
				}
				else
				{
					//����ҵ���߼������ؽ��
					neb::CJsonObject response_body;
					const std::string proxy_id = RequestProxyId(header.path, &data, proxy_manager);
					auto proxy = proxy_manager ? proxy_manager->GetProxy(proxy_id) : nullptr;
					auto require_proxy = [&]() -> bool {
						if (proxy)
						{
							return true;
						}
						response_body.Add("status", 404);
						response_body.Add("message", "Proxy not found");
						this->SendErrorResponse(connection, response_body, 404);
						return false;
						};
					
					if (m[1].str() == "create_proxy_server")
					{
						if (this->CreateProxyServer(response_body, data, proxy_manager))
						{
							this->SendSuccessResponse(connection, response_body);
						}
						else
						{
							WebControlServer::SendErrorResponse(connection, response_body);
						}
					}
					else if (m[1].str() == "remove_proxy_server")
					{
						if (this->RemoveProxyServer(response_body, data, proxy_manager))
						{
							this->SendSuccessResponse(connection, response_body);
						}
						else
						{
							WebControlServer::SendErrorResponse(connection, response_body);
						}
					}
					else if (m[1].str() == "add_whitelist_user")
					{
						if (this->AddWhitelistUser(response_body, data, proxy))
						{
							this->SendSuccessResponse(connection, response_body);
						}
						else
						{
							WebControlServer::SendErrorResponse(connection, response_body);
						}
					}
					else if (m[1].str() == "remove_whitelist_user")
					{
						if (this->RemoveWhitelistUser(response_body, data, proxy))
						{
							this->SendSuccessResponse(connection, response_body);
						}
						else
						{
							WebControlServer::SendErrorResponse(connection, response_body);
						}
					}
					else if (m[1].str() == "add_blacklist_user")
					{
						if (this->AddBlacklistUser(response_body, data, proxy))
						{
							this->SendSuccessResponse(connection, response_body);
						}
						else
						{
							WebControlServer::SendErrorResponse(connection, response_body);
						}
					}
					else if (m[1].str() == "remove_blacklist_user")
					{
						if (this->RemoveBlacklistUser(response_body, data, proxy))
						{
							this->SendSuccessResponse(connection, response_body);
						}
						else
						{
							WebControlServer::SendErrorResponse(connection, response_body);
						}
					}
					else if (m[1].str() == "set_user_proxy")
					{
						if (!require_proxy()) return 0;
						if (this->SetUserProxy(response_body, data, proxy))
						{
							this->SendSuccessResponse(connection, response_body);
						}
						else
						{
							WebControlServer::SendErrorResponse(connection, response_body);
						}
					}
					else if (m[1].str() == "remove_user_proxy")
					{
						if (!require_proxy()) return 0;
						if (this->RemoveUserProxy(response_body, data, proxy))
						{
							this->SendSuccessResponse(connection, response_body);
						}
						else
						{
							WebControlServer::SendErrorResponse(connection, response_body);
						}
					}
					else if (m[1].str() == "set_max_users")
					{
						if (!require_proxy()) return 0;
						if (this->SetMaxUsers(response_body, data, proxy_manager, proxy_id))
						{
							this->SendSuccessResponse(connection, response_body);
						}
						else
						{
							WebControlServer::SendErrorResponse(connection, response_body);
						}
					}
					else if (m[1].str() == "kick_player")
					{
						if (!require_proxy()) return 0;
						if (this->KickPlayer(response_body, data, proxy))
						{
							this->SendSuccessResponse(connection, response_body);
						}
						else
						{
							WebControlServer::SendErrorResponse(connection, response_body);
						}
					}
					else if (m[1].str() == "get_online_number_list")
					{
						if (!require_proxy()) return 0;
						if (this->GetUserNumberList(response_body, data, proxy_id))
						{
							this->SendSuccessResponse(connection, response_body);
						}
						else
						{
							WebControlServer::SendErrorResponse(connection, response_body);
						}
					}
					else if (m[1].str() == "set_motd")
					{
						if (!require_proxy()) return 0;
						if (this->SetMotd(response_body, data, proxy))
						{
							this->SendSuccessResponse(connection, response_body);
						}
						else
						{
							WebControlServer::SendErrorResponse(connection, response_body);
						}
					}
					else if (m[1].str() == "reload_motd")
					{
						if (!require_proxy()) return 0;
						if (this->ReloadMotd(response_body, proxy_manager, proxy_id))
						{
							this->SendSuccessResponse(connection, response_body);
						}
						else
						{
							WebControlServer::SendErrorResponse(connection, response_body);
						}
					}
					else
					{
						WebControlServer::SendErrorResponse(connection, 404, "Unknown API endpoint");
					}
				}
			}
		}
		else
		{
			WebControlServer::SendErrorResponse(connection, 404, "Unknown API endpoint");
		}
		return 0;
		});

	this->server.SetOptionsHandle([this](const RbsLib::Network::TCP::TCPConnection& connection, RbsLib::Network::HTTP::RequestHeader& header) -> int {
		//����OPTIONS������Ҫ����CORSԤ������
		RbsLib::Network::HTTP::ResponseHeader response;
		response.status = 204; // No Content
		response.headers.AddHeader("Access-Control-Allow-Origin", "*");
		response.headers.AddHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
		response.headers.AddHeader("Access-Control-Allow-Headers", "Content-Type, Authorize");
		response.headers.AddHeader("Access-Control-Max-Age", "86400"); // 24 hours
		connection.Send(response.ToBuffer());
		return 0;
		});
	std::thread([this]() {
		try
		{
			this->server.LoopWait(true, 3);
		}
		catch (const std::exception& e)
		{

		}
		}).detach();
	std::thread([this, proxy_manager]() {
		try
		{
			asio::co_spawn(this->io_context, this->TimeTaskUsers(proxy_manager), asio::detached);
			this->io_context.run();
		}
		catch (const std::exception& e)
		{
		}
		}).detach();
}

void WebControlServer::Stop(void)
{
	this->is_request_stop = true;
	this->server.StopAndThrowExceptionInLoopThread();
	this->io_context.stop();
}
