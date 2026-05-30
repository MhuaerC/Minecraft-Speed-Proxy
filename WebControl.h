#pragma once
#include <memory>
#include <chrono>
#include "rbslib/Network.h"
#include "proxy.h"
#include "ProxyManager.h"
#include "json/CJsonObject.h"
#include <list>
#include <map>
#include <shared_mutex>
#include "asio/import_asio.h"


/*
* ��ҳ���Ʒ����࣬�ṩ����WebAPI���Ʒ������Ĺ���
*/
class WebControlServer
{
protected:
	RbsLib::Network::HTTP::HTTPServer server;
	std::string user_token;
	//token����ʱ��
	std::chrono::system_clock::time_point token_expiry_time;
	std::shared_ptr<std::string> user_password;
	bool is_request_stop = false;
	std::map<std::string, std::list<std::pair<std::time_t, std::string>>> logs;
	int max_log_size = 100; //�����־����
	std::shared_mutex log_mutex; //��־������
	asio::io_context io_context;
	std::map<std::string, std::map<std::time_t, uint32_t>> time_online_users;
	std::shared_mutex time_online_users_mutex;
	std::shared_ptr<ProxyManager> proxy_manager;

	asio::awaitable<void> TimeTaskUsers(std::shared_ptr<ProxyManager> proxy_manager);

	static void SendTextResponse(const RbsLib::Network::TCP::TCPConnection& connection, const std::string& body, const std::string& content_type, int http_status_code = 200);
	static void SendErrorResponse(const RbsLib::Network::TCP::TCPConnection& connection, int status_code, const std::string& message = "");
	static void SendErrorResponse(const RbsLib::Network::TCP::TCPConnection& connection, const neb::CJsonObject& json, int http_status_code = 200);
	static void SendSuccessResponse(const RbsLib::Network::TCP::TCPConnection& connection, const neb::CJsonObject& json);
	static bool CheckToken(const std::string& cookie,const std::string& token,const std::chrono::system_clock::time_point& token_expiry_time);
	static void GetStatus(neb::CJsonObject& response, const ProxyServiceView& view, const std::shared_ptr<Proxy>& proxy_client);
	static void GetOnlineUsers(neb::CJsonObject& response, const std::shared_ptr<Proxy>& proxy_client);
	static void GetWhiteList(neb::CJsonObject& response, const std::shared_ptr<Proxy>& proxy_client);
	static void GetBlackList(neb::CJsonObject& response, const std::shared_ptr<Proxy>& proxy_client);
	static bool AddBlacklistUser(neb::CJsonObject& response,const neb::CJsonObject& request, const std::shared_ptr<Proxy>& proxy_client);
	static bool RemoveBlacklistUser(neb::CJsonObject& response, const neb::CJsonObject& request, const std::shared_ptr<Proxy>& proxy_client);
	static bool AddWhitelistUser(neb::CJsonObject& response, const neb::CJsonObject& request, const std::shared_ptr<Proxy>& proxy_client);
	static bool RemoveWhitelistUser(neb::CJsonObject& response, const neb::CJsonObject& request, const std::shared_ptr<Proxy>& proxy_client);
	static bool EnableWhitelist(neb::CJsonObject& response, const std::shared_ptr<Proxy>& proxy_client);
	static bool DisableWhitelist(neb::CJsonObject& response, const std::shared_ptr<Proxy>& proxy_client);
	static void GetUserProxyList(neb::CJsonObject& response, const std::shared_ptr<Proxy>& proxy_client);
	static bool SetUserProxy(neb::CJsonObject& response, const neb::CJsonObject& request, const std::shared_ptr<Proxy>& proxy_client);
	static bool RemoveUserProxy(neb::CJsonObject& response, const neb::CJsonObject& request, const std::shared_ptr<Proxy>& proxy_client);
	//��������������-1��ʾ������
	static bool SetMaxUsers(neb::CJsonObject& response, const neb::CJsonObject& request, const std::shared_ptr<ProxyManager>& proxy_manager, const std::string& proxy_id);
	static bool KickPlayer(neb::CJsonObject& response, const neb::CJsonObject& request, const std::shared_ptr<Proxy>& proxy_client);
	static void GetStartTime(neb::CJsonObject& response, const std::shared_ptr<Proxy>& proxy_client);
	bool GetUserNumberList(neb::CJsonObject& response, neb::CJsonObject& request, const std::string& proxy_id);
	void GetLogs(neb::CJsonObject& response, const std::string& proxy_id);
	static void GetMotd(neb::CJsonObject& response, const std::shared_ptr<Proxy>& proxy_client);
	static bool SetMotd(neb::CJsonObject& response, const neb::CJsonObject& request, const std::shared_ptr<Proxy>& proxy_client);
	static bool ReloadMotd(neb::CJsonObject& response, const std::shared_ptr<ProxyManager>& proxy_manager, const std::string& proxy_id);
	static void GetProxyServers(neb::CJsonObject& response, const std::shared_ptr<ProxyManager>& proxy_manager);
	static bool CreateProxyServer(neb::CJsonObject& response, const neb::CJsonObject& request, const std::shared_ptr<ProxyManager>& proxy_manager);
	bool RemoveProxyServer(neb::CJsonObject& response, const neb::CJsonObject& request, const std::shared_ptr<ProxyManager>& proxy_manager);
public:
	WebControlServer(const std::string& address, std::uint16_t port);
	~WebControlServer() noexcept;

	void SetUserPassword(const std::string& password);
	void AppendProxyLog(const std::string& proxy_id, const std::string& message);
	//�ڶ����߳�������������
	void Start(std::shared_ptr<ProxyManager>& proxy_manager);
	//ֹͣ���񲢵ȴ��������
	void Stop(void);

};
