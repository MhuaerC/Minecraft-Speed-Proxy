#pragma once

#include <string>
#include <string_view>

inline constexpr std::string_view kWebPanelHtml = R"html(<!doctype html>
<html lang="zh-CN">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Minecraft 加速代理管理面板</title>
  <link rel="stylesheet" href="/panel.css">
</head>
<body>
  <div class="app">
    <header class="topbar">
      <div>
        <div class="brand">Minecraft 加速代理</div>
        <div class="subtle">管理面板</div>
      </div>
      <div class="toolbar">
        <select id="activeProxySelect" class="server-select" aria-label="当前服务器"></select>
        <span id="connectionState" class="status-pill status-warn">未连接</span>
        <button id="refreshBtn" class="secondary" type="button">刷新</button>
        <button id="logoutBtn" class="ghost" type="button">退出</button>
      </div>
    </header>

    <section id="loginView" class="section login-shell">
      <form id="loginForm" class="login-card">
        <label for="passwordInput">管理密码</label>
        <input id="passwordInput" type="password" autocomplete="current-password" placeholder="请输入管理密码">
        <button class="primary" type="submit">登录</button>
        <div id="loginMessage" class="status-line"></div>
      </form>
    </section>

    <section id="panelView" class="hidden">
      <nav class="tabs" aria-label="页面导航">
        <button type="button" class="tab active" data-tab="overview">概览</button>
        <button type="button" class="tab" data-tab="servers">服务器</button>
        <button type="button" class="tab" data-tab="users">在线玩家</button>
        <button type="button" class="tab" data-tab="lists">名单</button>
        <button type="button" class="tab" data-tab="proxies">玩家代理</button>
        <button type="button" class="tab" data-tab="motd">MOTD</button>
        <button type="button" class="tab" data-tab="logs">日志</button>
        <button type="button" class="tab" data-tab="settings">设置</button>
      </nav>

      <section id="overview" class="section tab-panel">
        <div class="metric-grid">
          <div class="metric"><div class="label">在线人数</div><div id="metricOnline" class="value">-</div></div>
          <div class="metric"><div class="label">最大人数</div><div id="metricMax" class="value">-</div></div>
          <div class="metric"><div class="label">白名单</div><div id="metricWhitelist" class="value">-</div></div>
          <div class="metric"><div class="label">默认代理</div><div id="metricProxy" class="value long-text">-</div></div>
          <div class="metric"><div class="label">运行时长</div><div id="metricUptime" class="value">-</div></div>
          <div class="metric"><div class="label">启动时间</div><div id="metricStart" class="value long-text">-</div></div>
        </div>

        <div class="grid-2">
          <div class="panel-box">
            <div class="toolbar">
              <button id="toggleWhitelistBtn" class="secondary" type="button">切换白名单</button>
              <button id="reloadMotdBtn" class="secondary" type="button">重载 MOTD</button>
            </div>
            <div id="overviewMessage" class="status-line"></div>
          </div>

          <div class="panel-box">
            <form id="maxPlayerForm">
              <label for="maxPlayerInput">最大玩家数</label>
              <div class="form-row">
                <input id="maxPlayerInput" type="number" step="1" placeholder="-1">
                <button class="primary" type="submit">保存</button>
              </div>
            </form>
          </div>
        </div>
      </section>

      <section id="servers" class="section tab-panel hidden">
        <div class="grid-2">
          <div class="panel-box">
            <form id="serverForm" class="stack-form">
              <label for="serverName">服务器名称</label>
              <input id="serverName" type="text" placeholder="例如：生存服">
              <div class="form-row">
                <input id="serverLocalAddress" type="text" placeholder="0.0.0.0">
                <input id="serverLocalPort" type="number" min="1" max="65535" step="1" placeholder="本地端口">
              </div>
              <div class="form-row">
                <input id="serverRemoteAddress" type="text" placeholder="远程地址">
                <input id="serverRemotePort" type="number" min="1" max="65535" step="1" placeholder="远程端口">
              </div>
              <div class="form-row">
                <input id="serverMaxPlayer" type="number" step="1" placeholder="-1">
                <input id="serverMotdPath" type="text" placeholder="MOTD 路径">
              </div>
              <button class="primary" type="submit">创建</button>
            </form>
          </div>
          <div class="panel-box">
            <table>
              <thead>
                <tr>
                  <th>名称</th>
                  <th>监听地址</th>
                  <th>目标服务器</th>
                  <th>在线</th>
                  <th>操作</th>
                </tr>
              </thead>
              <tbody id="serversTable"></tbody>
            </table>
          </div>
        </div>
      </section>

      <section id="users" class="section tab-panel hidden">
        <div class="panel-box">
          <table>
            <thead>
              <tr>
                <th>玩家</th>
                <th>UUID</th>
                <th>IP</th>
                <th>流量</th>
                <th>上线时间</th>
                <th>代理</th>
                <th>操作</th>
              </tr>
            </thead>
            <tbody id="usersTable"></tbody>
          </table>
        </div>
      </section>

      <section id="lists" class="section tab-panel hidden">
        <div class="grid-2">
          <div class="panel-box">
            <div class="toolbar">
              <strong>白名单</strong>
              <button id="whitelistToggleBtn" class="secondary" type="button">切换</button>
            </div>
            <form id="whiteAddForm" class="form-row">
              <input id="whiteAddInput" type="text" placeholder="玩家用户名">
              <button class="primary" type="submit">添加</button>
            </form>
            <div id="whiteList" class="list-stack"></div>
          </div>
          <div class="panel-box">
            <strong>黑名单</strong>
            <form id="blackAddForm" class="form-row">
              <input id="blackAddInput" type="text" placeholder="玩家用户名">
              <button class="primary" type="submit">添加</button>
            </form>
            <div id="blackList" class="list-stack"></div>
          </div>
        </div>
      </section>

      <section id="proxies" class="section tab-panel hidden">
        <div class="grid-2">
          <div class="panel-box">
            <div class="label">默认代理</div>
            <div id="defaultProxyText" class="value long-text">-</div>
            <form id="userProxyForm" class="stack-form">
              <label for="proxyUsername">玩家专属代理</label>
              <input id="proxyUsername" type="text" placeholder="玩家用户名">
              <div class="form-row">
                <input id="proxyAddress" type="text" placeholder="地址">
                <input id="proxyPort" type="number" min="1" max="65535" step="1" placeholder="端口">
              </div>
              <button class="primary" type="submit">保存</button>
            </form>
          </div>
          <div class="panel-box">
            <table>
              <thead>
                <tr>
                  <th>玩家</th>
                  <th>目标</th>
                  <th>操作</th>
                </tr>
              </thead>
              <tbody id="proxyTable"></tbody>
            </table>
          </div>
        </div>
      </section>

      <section id="motd" class="section tab-panel hidden">
        <div class="toolbar">
          <button id="motdPrettyBtn" class="secondary" type="button">格式化</button>
          <button id="motdReloadBtn" class="secondary" type="button">重载</button>
          <button id="motdSaveBtn" class="primary" type="button">保存</button>
        </div>
        <textarea id="motdEditor" spellcheck="false"></textarea>
        <div id="motdMessage" class="status-line"></div>
      </section>

      <section id="logs" class="section tab-panel hidden">
        <div class="panel-box">
          <table>
            <thead>
              <tr>
                <th>时间</th>
                <th>消息</th>
              </tr>
            </thead>
            <tbody id="logsTable"></tbody>
          </table>
        </div>
      </section>

      <section id="settings" class="section tab-panel hidden">
        <div class="grid-2">
          <div class="panel-box">
            <div class="label">服务器</div>
            <div id="serverInfo" class="long-text">-</div>
          </div>
          <div class="panel-box">
            <div class="label">状态</div>
            <div id="panelStatus" class="long-text">-</div>
          </div>
        </div>
      </section>
    </section>
  </div>

  <script src="/panel.js"></script>
</body>
</html>)html";

inline constexpr std::string_view kWebPanelCss = R"css(:root {
  color-scheme: dark;
  --bg: #0f1410;
  --panel: #18211b;
  --panel-soft: #1f2a22;
  --border: #314236;
  --text: #e7f0e8;
  --muted: #95a79a;
  --accent: #49d17d;
  --accent-soft: #203629;
  --success: #4ad18a;
  --warning: #d1a649;
  --danger: #ef6b63;
}

* { box-sizing: border-box; }

body {
  margin: 0;
  background: var(--bg);
  color: var(--text);
  font: 14px/1.45 system-ui, -apple-system, BlinkMacSystemFont, "Segoe UI", "Microsoft YaHei", sans-serif;
}

button, input, select, textarea {
  font: inherit;
}

button {
  border: 1px solid var(--border);
  background: var(--panel);
  color: var(--text);
  border-radius: 6px;
  padding: 8px 12px;
  cursor: pointer;
  transition: background-color 0.15s ease, border-color 0.15s ease, transform 0.15s ease;
}

button:hover {
  border-color: var(--accent);
  transform: translateY(-1px);
}

button.primary {
  background: var(--accent);
  border-color: var(--accent);
  color: #0f1410;
}

button.secondary {
  background: var(--accent-soft);
  border-color: #335442;
}

button.ghost {
  background: transparent;
}

button.danger {
  background: #291816;
  border-color: #5a2f2b;
  color: var(--danger);
}

input, select, textarea {
  width: 100%;
  border: 1px solid var(--border);
  border-radius: 6px;
  padding: 8px 10px;
  background: var(--panel);
  color: var(--text);
}

.server-select {
  width: min(260px, 100%);
}

textarea {
  min-height: 280px;
  resize: vertical;
  font-family: ui-monospace, SFMono-Regular, Consolas, "Liberation Mono", monospace;
}

label, .label {
  display: block;
  font-size: 12px;
  color: var(--muted);
  margin-bottom: 6px;
}

.app {
  max-width: 1500px;
  margin: 0 auto;
  padding: 16px;
}

.topbar,
.section,
.panel-box {
  border: 1px solid var(--border);
  border-radius: 6px;
  background: var(--panel);
  box-shadow: 0 14px 36px rgba(0, 0, 0, 0.22);
}

.topbar {
  display: flex;
  justify-content: space-between;
  align-items: flex-start;
  gap: 12px;
  padding: 14px 16px;
}

.brand {
  font-size: 18px;
  font-weight: 700;
  color: var(--accent);
}

.subtle,
.note,
.status-line {
  color: var(--muted);
}

.toolbar {
  display: flex;
  align-items: center;
  gap: 8px;
  flex-wrap: wrap;
}

.status-pill {
  display: inline-flex;
  align-items: center;
  border-radius: 6px;
  padding: 4px 8px;
  border: 1px solid var(--border);
  background: var(--panel);
  font-size: 12px;
}

.status-ok {
  color: var(--success);
  border-color: #335442;
  background: #1f2e24;
}

.status-warn {
  color: var(--warning);
  border-color: #5b4b2d;
  background: #2c261a;
}

.status-bad {
  color: var(--danger);
  border-color: #5a2f2b;
  background: #291816;
}

.hidden {
  display: none !important;
}

.login-shell {
  margin-top: 16px;
  min-height: calc(100vh - 112px);
  display: grid;
  place-items: center;
  border: 0;
  background: transparent;
  box-shadow: none;
  padding: 24px;
}

.login-card {
  width: min(420px, 100%);
  max-width: 420px;
  border: 1px solid var(--border);
  border-radius: 6px;
  background: var(--panel);
  padding: 28px;
  box-shadow: 0 18px 46px rgba(0, 0, 0, 0.28);
}

.tabs {
  display: flex;
  flex-wrap: wrap;
  gap: 6px;
  margin-top: 12px;
}

.tabs button.active {
  background: var(--accent);
  border-color: var(--accent);
  color: #0f1410;
  font-weight: 700;
}

.section {
  margin-top: 12px;
  padding: 16px;
}

.metric-grid,
.grid-2 {
  display: grid;
  gap: 12px;
}

.metric-grid {
  grid-template-columns: repeat(auto-fit, minmax(180px, 1fr));
}

.grid-2 {
  margin-top: 12px;
  grid-template-columns: repeat(auto-fit, minmax(320px, 1fr));
}

.metric {
  border: 1px solid var(--border);
  border-left: 3px solid var(--accent);
  border-radius: 6px;
  background: var(--panel-soft);
  padding: 12px;
}

.metric .value {
  font-size: 22px;
  font-weight: 700;
  overflow-wrap: anywhere;
}

.panel-box {
  padding: 12px;
}

.form-row,
.stack-form {
  margin-top: 10px;
}

.form-row {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(120px, 1fr));
  gap: 8px;
}

.stack-form > * + * {
  margin-top: 8px;
}

.list-stack {
  margin-top: 10px;
  display: grid;
  gap: 8px;
}

.list-item {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 8px;
  border: 1px solid var(--border);
  border-radius: 6px;
  padding: 8px 10px;
  background: var(--panel-soft);
}

table {
  width: 100%;
  border-collapse: collapse;
}

thead th {
  text-align: left;
  padding: 8px 6px;
  border-bottom: 1px solid var(--border);
  color: var(--muted);
  font-size: 12px;
}

tbody td {
  padding: 8px 6px;
  border-bottom: 1px solid var(--border);
  vertical-align: top;
}

tbody tr:hover {
  background: #223129;
}

.long-text {
  overflow-wrap: anywhere;
  word-break: break-word;
}

.action-cell {
  white-space: nowrap;
}
)css";

inline auto GetWebPanelJs() -> const std::string&
{
	static const std::string js = []() {
		std::string value;
		value.reserve(25737);
		value += R"paneljs((() => {
  const state = {
    token: localStorage.getItem("msp_panel_token") || "",
    expiry: Number(localStorage.getItem("msp_panel_expiry") || "0"),
    loading: false,
    data: {},
    servers: [],
    selectedProxyId: localStorage.getItem("msp_selected_proxy") || "",
    activeTab: "overview",
    timer: null
  };

  const $ = (selector) => document.querySelector(selector);
  const els = {
    loginView: $("#loginView"),
    panelView: $("#panelView"),
    loginForm: $("#loginForm"),
    loginMessage: $("#loginMessage"),
    loginInput: $("#passwordInput"),
    connectionState: $("#connectionState"),
    activeProxySelect: $("#activeProxySelect"),
    refreshBtn: $("#refreshBtn"),
    logoutBtn: $("#logoutBtn"),
    overviewMessage: $("#overviewMessage"),
    panelStatus: $("#panelStatus"),
    serverInfo: $("#serverInfo"),
    maxPlayerInput: $("#maxPlayerInput"),
    maxPlayerForm: $("#maxPlayerForm"),
    toggleWhitelistBtn: $("#toggleWhitelistBtn"),
    whitelistToggleBtn: $("#whitelistToggleBtn"),
    reloadMotdBtn: $("#reloadMotdBtn"),
    usersTable: $("#usersTable"),
    whiteList: $("#whiteList"),
    blackList: $("#blackList"),
    whiteAddForm: $("#whiteAddForm"),
    whiteAddInput: $("#whiteAddInput"),
    blackAddForm: $("#blackAddForm"),
    blackAddInput: $("#blackAddInput"),
    serverForm: $("#serverForm"),
    serverName: $("#serverName"),
    serverLocalAddress: $("#serverLocalAddress"),
    serverLocalPort: $("#serverLocalPort"),
    serverRemoteAddress: $("#serverRemoteAddress"),
    serverRemotePort: $("#serverRemotePort"),
    serverMaxPlayer: $("#serverMaxPlayer"),
    serverMotdPath: $("#serverMotdPath"),
    serversTable: $("#serversTable"),
    defaultProxyText: $("#defaultProxyText"),
    userProxyForm: $("#userProxyForm"),
    proxyUsername: $("#proxyUsername"),
    proxyAddress: $("#proxyAddress"),
    proxyPort: $("#proxyPort"),
    proxyTable: $("#proxyTable"),
    motdEditor: $("#motdEditor"),
    motdMessage: $("#motdMessage"),
    motdPrettyBtn: $("#motdPrettyBtn"),
    motdReloadBtn: $("#motdReloadBtn"),
    motdSaveBtn: $("#motdSaveBtn"),
    logsTable: $("#logsTable"),
    metricOnline: $("#metricOnline"),
    metricMax: $("#metricMax"),
    metricWhitelist: $("#metricWhitelist"),
    metricProxy: $("#metricProxy"),
    metricUptime: $("#metricUptime"),
    metricStart: $("#metricStart")
  };

  function setMessage(node, message, kind = "") {
    if (!node) return;
    node.textContent = message || "";
    node.className = "status-line" + (kind ? " " + kind : "");
  }

  function formatBytes(value) {
    const n = Number(value || 0);
    if (n >= 1024 ** 3) return `${(n / 1024 ** 3).toFixed(2)} GB`;
    if (n >= 1024 ** 2) return `${(n / 1024 ** 2).toFixed(2)} MB`;
    if (n >= 1024) return `${(n / 1024).toFixed(2)} KB`;
    return `${n} B`;
  }

  function formatTime(value) {
    const n = Number(value || 0);
    if (!n) return "-";
    return new Date(n * 1000).toLocaleString();
  }

  function formatDuration(seconds) {
    const total = Math.max(0, Math.floor(Number(seconds || 0)));
    const days = Math.floor(total / 86400);
    const hours = Math.floor((total % 86400) / 3600);
    const minutes = Math.floor((total % 3600) / 60);
    const parts = [];
    if (days) parts.push(`${days}天`);
    if (hours || parts.length) parts.push(`${hours}小时`);
    parts.push(`${minutes}分钟`);
    return parts.join(" ");
  }

  function formatJson(value) {
    if (value === null || value === undefined) return "";
    if (typeof value === "string") {
      try {
        return JSON.stringify(JSON.parse(value), null, 2);
      } catch {
        return value;
      }
    }
    return JSON.stringify(value, null, 2);
  }

  function translateMessage(message) {
    const text = String(message || "");
    const map = {
      "Invalid password": "密码错误",
      "Unauthorized": "未授权",
      "Request failed": "请求失败",
      "Login failed": "登录失败",
      "Proxy not found": "未找到代理",
      "Server not found": "未找到服务器",
      "Missing server fields": "请填写服务器必填项",
      "Missing proxy fields": "请填写代理必填项",
      "Missing 'username' field in request": "请求中缺少用户名",
      "Missing 'remote_address' field": "请求中缺少远程地址",
      "Missing 'proxy_id' field": "请求中缺少代理 ID",
      "Missing 'username', 'proxy_address' or 'proxy_port' field in request": "请填写玩家名、代理地址和端口",
      "Invalid 'proxy_port' value, must be in range 1-65535": "代理端口必须在 1-65535 之间",
      "Invalid 'local_port' value": "本地端口无效",
      "Invalid 'remote_port' value": "远程端口无效",
      "Invalid 'max_users' value": "最大玩家数无效",
      "Missing or invalid 'motd' field in request": "MOTD 配置无效",
    };
    return map[text] || text;
  }

  function setConnectionState(text, kind) {
    els.connectionState.textContent = text;
    els.connectionState.className = `status-pill ${kind}`;
  }

  function saveSession(token, expiry) {
    state.token = token || "";
    state.expiry = Number(expiry || 0);
    localStorage.setItem("msp_panel_token", state.token);
    localStorage.setItem("msp_panel_expiry", String(state.expiry));
  }

  function clearSession() {
    state.token = "";
    state.expiry = 0;
    localStorage.removeItem("msp_panel_token");
    localStorage.removeItem("msp_panel_expiry");
  }

  function isSessionValid() {
    return !!state.token && (!state.expiry || Date.now() / 1000 < state.expiry - 5);
  }

  function setSelectedProxyId(proxyId) {
    state.selectedProxyId = proxyId || "";
    if (state.selectedProxyId) {
      localStorage.setItem("msp_selected_proxy", state.selectedProxyId);
    } else {
      localStorage.removeItem("msp_selected_proxy");
    }
  }

  function withSelectedProxy(path) {
    if (!state.selectedProxyId) return path;
    const separator = path.includes("?") ? "&" : "?";
    return `${path}${separator}proxy_id=${encodeURIComponent(state.selectedProxyId)}`;
  }

  function showLogin(message = "") {
    els.loginView.classList.remove("hidden");
    els.panelView.classList.add("hidden");
    setConnectionState("未连接", "status-warn");
    setMessage(els.loginMessage, message, message ? "status-bad" : "");
  }

  function showPanel() {
    els.loginView.classList.add("hidden");
    els.panelView.classList.remove("hidden");
    setConnectionState("已连接", "status-ok");
  }

  async function request(path, options = {}) {
    const headers = new Headers(options.headers || {});
    const isLogin = path === "/api/login";
    if (!isLogin && state.token) {
      headers.set("Authorize", state.token);
    }
    if (options.body && !headers.has("Content-Type") && !(options.body instanceof FormData)) {
      headers.set("Content-Type", "application/json");
    }
    const response = await fetch(path, {
      method: options.method || "GET",
      headers,
      body: options.body,
    });
    const text = await response.text();
    let payload = {};
    if (text) {
      try {
        payload = JSON.parse(text);
      } catch {
        payload = { status: response.status, message: text };
      }
    }
    if (response.status === 401 || payload.status === 401) {
      clearSession();
      showLogin("未授权，请重新登录");
      throw new Error(translateMessage(payload.message || "未授权"));
    }
    return payload;
  }

  async function apiOk(path, options = {}) {
    const payload = await request(path, options);
    if (payload && typeof payload.status === "number" && payload.status !== 200) {
      throw new Error(translateMessage(payload.message || "请求失败"));
    }
    return payload;
  }

  function activateTab(name) {
    state.activeTab = name;
    document.querySelectorAll(".tab").forEach((btn) => {
      btn.classList.toggle("active", btn.dataset.tab === name);
    });
    document.querySelectorAll(".tab-panel").forEach((panel) => {
      panel.classList.toggle("hidden", panel.id !== name);
    });
  }

  function renderOverview(status) {
    const online = Number(status.online_users || 0);
    const maxPlayers = Number(status.max_player ?? -1);
    const whitelistOn = !!status.whitelist_status;
    els.metricOnline.textContent = String(online);
    els.metricMax.textContent = maxPlayers < 0 ? "无限制" : String(maxPlayers);
    els.metricWhitelist.textContent = whitelistOn ? "开启" : "关闭";
    els.metricProxy.textContent = status.default_proxy || "-";
    els.metricUptime.textContent = formatDuration(status.uptime_seconds || 0);
    els.metricStart.textContent = formatTime(status.start_time);
    els.maxPlayerInput.value = String(maxPlayers);
    els.defaultProxyText.textContent = status.default_proxy || "-";
    els.serverInfo.textContent = [
      `服务器: ${status.proxy_name || status.proxy_id || "-"}`,
      `监听: ${status.listen_endpoint || "-"}`,
      `在线: ${online}`,
      `最大玩家数: ${maxPlayers < 0 ? "无限制" : maxPlayers}`,
      `白名单: ${whitelistOn ? "开启" : "关闭"}`,
      `默认代理: ${status.default_proxy || "-"}`,
      `启动时间: ${formatTime(status.start_time)}`
    ].join(" | ");
  }

  function renderUsers(users) {
    els.usersTable.innerHTML = "";
    if (!users.length) {
      els.usersTable.innerHTML = '<tr><td colspan="7" class="note">暂无在线玩家</td></tr>';
      return;
    }
    els.usersTable.innerHTML = users.map((user) => `
      <tr>
        <td>${escapeHtml(user.username || "")}</td>
        <td class="long-text">${escapeHtml(user.uuid || "")}</td>
        <td>${escapeHtml(user.ip || "")}</td>
        <td>${formatBytes(user.current_proxy_flow || user.current_proxy_size || 0)}</td>
        <td>${formatTime(user.online_time_stamp || user.connect_time || 0)}</td>
        <td class="long-text">${escapeHtml(user.proxy_target || "-")}</td>
        <td class="action-cell">
          <button class="danger" type="button" data-action="kick-user" data-username="${escapeAttr(user.username || "")}">踢出</button>
        </td>
      </tr>
    `).join("");
  }

  function renderList(container, values, action) {
    container.innerHTML = "";
    if (!values.length) {
      container.innerHTML = '<div class="note">暂无数据</div>';
      return;
    }
    container.innerHTML = values.map((value) => `
      <div class="list-item">
        <div class="long-text">${escapeHtml(value)}</div>
        <button class="danger" type="button" data-action="${action}" data-value="${escapeAttr(value)}">移除</button>
      </div>
    `).join("");
  }

  function renderProxies(data) {
    const proxies = Array.isArray(data.user_proxies) ? data.user_proxies : [];
    els.proxyTable.innerHTML = "";
    if (!proxies.length) {
      els.proxyTable.innerHTML = '<tr><td colspan="3" class="note">暂无玩家代理</td></tr>';
      return;
    }
    els.proxyTable.innerHTML = proxies.map((item) => `
      <tr>
        <td>${escapeHtml(item.username || "")}</td>
        <td class="long-text">${escapeHtml(`${item.proxy_target_addr || ""}:${item.proxy_target_port || ""}`)}</td>
        <td class="action-cell">
          <button class="danger" type="button" data-action="remove-user-proxy" data-username="${escapeAttr(item.username || "")}">删除</button>
        </td>
      </tr>
    `).join("");
  }

  function renderServers(data) {
    const servers = Array.isArray(data.proxies) ? data.proxies : [];
    state.servers = servers;
    const exists = servers.some((item) => item.id === state.selectedProxyId);
    if (!exists) {
      setSelectedProxyId(servers[0]?.id || "");
    }
    els.activeProxySelect.innerHTML = servers.length
      ? servers.map((item) => `<option value="${escapeAttr(item.id || "")}">${escapeHtml(item.name || item.id || "-")} (${escapeHtml(item.listen_endpoint || "")})</option>`).join("")
      : '<option value="">暂无服务器</option>';
    els.activeProxySelect.value = state.selectedProxyId;
    els.serversTable.innerHTML = "";
    if (!servers.length) {
      els.serversTable.innerHTML = '<tr><td colspan="5" class="note">暂无服务器</td></tr>';
      return;
    }
    els.serversTable.innerHTML = servers.map((item) => `
      <tr>
        <td class="long-text">${escapeHtml(item.name || item.id || "")}</td>
        <td class="long-text">${escapeHtml(item.listen_endpoint || `${item.local_address || ""}:${item.local_port || ""}`)}</td>
        <td class="long-text">${escapeHtml(item.default_proxy || `${item.remote_address || ""}:${item.remote_port || ""}`)}</td>
        <td>${Number(item.online_users || 0)}</td>
        <td class="action-cell">
          <button class="secondary" type="button" data-action="select-proxy" data-proxy-id="${escapeAttr(item.id || "")}">选择</button>
          <button class="danger" type="button" data-action="remove-proxy-server" data-proxy-id="${escapeAttr(item.id || "")}">删除</button>
        </td>
      </tr>
    `).join("");
  }

  function renderMotd(data) {
    if (data && data.motd !== undefined) {
      els.motdEditor.value = formatJson(data.motd);
    } else if (!els.motdEditor.value) {
      els.motdEditor.value = "{}";
    }
  }

  function renderLogs(logs) {
)paneljs";
		value += R"paneljs(    els.logsTable.innerHTML = "";
    if (!logs.length) {
      els.logsTable.innerHTML = '<tr><td colspan="2" class="note">暂无日志</td></tr>';
      return;
    }
    els.logsTable.innerHTML = logs.map((log) => `
      <tr>
        <td>${formatTime(log.timestamp || 0)}</td>
        <td class="long-text">${escapeHtml(log.message || "")}</td>
      </tr>
    `).join("");
  }

  async function refreshAll(silent = false) {
    if (!isSessionValid()) {
      showLogin();
      return;
    }
    if (state.loading) return;
    state.loading = true;
    if (!silent) setMessage(els.panelStatus, "加载中...");
    try {
      const servers = await apiOk("/api/get_proxy_servers");
      renderServers(servers);
      if (!state.selectedProxyId) {
        state.data = { servers };
        els.metricOnline.textContent = "-";
        els.metricMax.textContent = "-";
        els.metricWhitelist.textContent = "-";
        els.metricProxy.textContent = "-";
        els.metricUptime.textContent = "-";
        els.metricStart.textContent = "-";
        els.defaultProxyText.textContent = "-";
        els.serverInfo.textContent = "未选择服务器";
        renderUsers([]);
        renderProxies({ user_proxies: [] });
        renderLogs([]);
        setMessage(els.overviewMessage, "未选择服务器");
        setMessage(els.panelStatus, "就绪");
        setConnectionState("已连接", "status-ok");
        return;
      }
      const [status, users, white, black, proxies, motd, logs] = await Promise.all([
        apiOk(withSelectedProxy("/api/get_status")),
        apiOk(withSelectedProxy("/api/get_online_users")),
        apiOk("/api/get_whitelist"),
        apiOk("/api/get_blacklist"),
        apiOk(withSelectedProxy("/api/get_user_proxies")),
        apiOk(withSelectedProxy("/api/get_motd")),
        apiOk(withSelectedProxy("/api/get_logs")),
      ]);
      state.data = { servers, status, users, white, black, proxies, motd, logs };
      renderOverview(status);
      renderUsers(Array.isArray(users.online_users) ? users.online_users : []);
      renderList(els.whiteList, Array.isArray(white.white_list) ? white.white_list : [], "remove-whitelist-user");
      renderList(els.blackList, Array.isArray(black.black_list) ? black.black_list : [], "remove-blacklist-user");
      renderProxies(proxies);
      renderMotd(motd);
      renderLogs(Array.isArray(logs.logs) ? logs.logs : []);
      setMessage(els.overviewMessage, `已更新 ${new Date().toLocaleTimeString()}`);
      setMessage(els.panelStatus, "就绪");
      setConnectionState("已连接", "status-ok");
      if (status.whitelist_status) {
        els.toggleWhitelistBtn.textContent = "关闭白名单";
        els.whitelistToggleBtn.textContent = "关闭";
      } else {
        els.toggleWhitelistBtn.textContent = "开启白名单";
        els.whitelistToggleBtn.textContent = "开启";
      }
    } catch (error) {
      setMessage(els.panelStatus, error.message, "status-bad");
      if (String(error.message || "").includes("Unauthorized")) {
        clearSession();
        showLogin("未授权");
      }
    } finally {
      state.loading = false;
    }
  }

  async function handleLogin(event) {
    event.preventDefault();
    const password = els.loginInput.value.trim();
    if (!password) {
      setMessage(els.loginMessage, "请输入密码", "status-bad");
      return;
    }
    setMessage(els.loginMessage, "正在登录...");
    try {
      const result = await request("/api/login", {
        method: "POST",
        body: JSON.stringify({ password }),
      });
      if (result.status !== 200 || !result.token) {
        throw new Error(result.message || "登录失败");
      }
      saveSession(result.token, result.token_expiry_time);
      els.loginInput.value = "";
      showPanel();
      setMessage(els.loginMessage, "");
      await refreshAll();
    } catch (error) {
      setMessage(els.loginMessage, error.message, "status-bad");
    }
  }

  async function logout() {
    if (!state.token) {
      showLogin();
      return;
    }
    try {
      await request("/api/logout");
    } catch (_) {
      // Ignore logout failures; local session is cleared below.
    }
    clearSession();
    showLogin("已退出登录");
  }

  async function kickUser(username) {
    await apiOk(withSelectedProxy("/api/kick_player"), {
      method: "POST",
      body: JSON.stringify({ username }),
    });
    await refreshAll(true);
  }

  async function removeListUser(endpoint, username) {
    await apiOk(endpoint, {
      method: "POST",
      body: JSON.stringify({ username }),
    });
    await refreshAll(true);
  }

  async function addListUser(endpoint, username) {
    await apiOk(endpoint, {
      method: "POST",
      body: JSON.stringify({ username }),
    });
    await refreshAll(true);
  }

  async function setUserProxy(username, address, port) {
    await apiOk(withSelectedProxy("/api/set_user_proxy"), {
      method: "POST",
      body: JSON.stringify({
        username,
        proxy_address: address,
        proxy_port: Number(port),
      }),
    });
    await refreshAll(true);
  }

  async function removeUserProxy(username) {
    await apiOk(withSelectedProxy("/api/remove_user_proxy"), {
      method: "POST",
      body: JSON.stringify({ username }),
    });
    await refreshAll(true);
  }

  async function createProxyServer(payload) {
    const result = await apiOk("/api/create_proxy_server", {
      method: "POST",
      body: JSON.stringify(payload),
    });
    setSelectedProxyId(result.proxy_id || result.id || state.selectedProxyId);
    await refreshAll(true);
  }

  async function removeProxyServer(proxyId) {
    await apiOk("/api/remove_proxy_server", {
      method: "POST",
      body: JSON.stringify({ proxy_id: proxyId }),
    });
    if (state.selectedProxyId === proxyId) {
      setSelectedProxyId("");
    }
    await refreshAll(true);
  }

  async function saveMaxPlayers(value) {
    await apiOk(withSelectedProxy("/api/set_max_users"), {
      method: "POST",
      body: JSON.stringify({ max_users: Number(value) }),
    });
    await refreshAll(true);
  }

  async function toggleWhitelist() {
    const on = !!state.data?.status?.whitelist_status;
    await apiOk(on ? "/api/disable_whitelist" : "/api/enable_whitelist");
    await refreshAll(true);
  }

  async function reloadMotd() {
    await apiOk(withSelectedProxy("/api/reload_motd"), { method: "POST", body: "{}" });
    await refreshAll(true);
  }

  async function saveMotd() {
    const text = els.motdEditor.value.trim();
    let parsed;
    try {
      parsed = JSON.parse(text || "{}");
    } catch (error) {
      setMessage(els.motdMessage, error.message, "status-bad");
      return;
    }
    await apiOk(withSelectedProxy("/api/set_motd"), {
      method: "POST",
      body: JSON.stringify({ motd: parsed }),
    });
    setMessage(els.motdMessage, "保存成功", "status-ok");
    await refreshAll(true);
  }

  function escapeHtml(text) {
    return String(text)
      .replaceAll("&", "&amp;")
      .replaceAll("<", "&lt;")
      .replaceAll(">", "&gt;")
      .replaceAll('"', "&quot;")
      .replaceAll("'", "&#39;");
  }

  function escapeAttr(text) {
    return escapeHtml(text).replaceAll("\n", " ");
  }

  document.addEventListener("click", async (event) => {
    const button = event.target.closest("button");
    if (!button) return;
    const action = button.dataset.action;
    if (!action) return;
    try {
      if (action === "kick-user") {
        await kickUser(button.dataset.username || "");
      } else if (action === "remove-whitelist-user") {
        await removeListUser("/api/remove_whitelist_user", button.dataset.value || "");
      } else if (action === "remove-blacklist-user") {
        await removeListUser("/api/remove_blacklist_user", button.dataset.value || "");
      } else if (action === "remove-user-proxy") {
        await removeUserProxy(button.dataset.username || "");
      } else if (action === "select-proxy") {
        setSelectedProxyId(button.dataset.proxyId || "");
        await refreshAll(true);
      } else if (action === "remove-proxy-server") {
        const proxyId = button.dataset.proxyId || "";
        if (proxyId && confirm(`确定删除 ${proxyId}？`)) {
          await removeProxyServer(proxyId);
        }
      }
    } catch (error) {
      setMessage(els.panelStatus, error.message, "status-bad");
    }
  });

  document.querySelectorAll(".tab").forEach((button) => {
    button.addEventListener("click", () => activateTab(button.dataset.tab || "overview"));
  });

  els.activeProxySelect.addEventListener("change", async () => {
    setSelectedProxyId(els.activeProxySelect.value);
    await refreshAll(true);
  });
  els.loginForm.addEventListener("submit", handleLogin);
  els.refreshBtn.addEventListener("click", () => refreshAll());
  els.logoutBtn.addEventListener("click", logout);
  els.serverForm.addEventListener("submit", async (event) => {
    event.preventDefault();
    const payload = {
      name: els.serverName.value.trim(),
      local_address: els.serverLocalAddress.value.trim() || "0.0.0.0",
      local_port: Number(els.serverLocalPort.value),
      remote_address: els.serverRemoteAddress.value.trim(),
      remote_port: Number(els.serverRemotePort.value || 25565),
      max_player: Number(els.serverMaxPlayer.value || -1),
      motd_path: els.serverMotdPath.value.trim(),
    };
    if (!payload.local_port || !payload.remote_address || !payload.remote_port) {
      setMessage(els.panelStatus, "请填写服务器必填项", "status-bad");
      return;
    }
    try {
      await createProxyServer(payload);
      els.serverName.value = "";
      els.serverLocalAddress.value = "";
      els.serverLocalPort.value = "";
      els.serverRemoteAddress.value = "";
      els.serverRemotePort.value = "";
      els.serverMaxPlayer.value = "";
      els.serverMotdPath.value = "";
      setMessage(els.panelStatus, "服务器创建成功", "status-ok");
    } catch (error) {
      setMessage(els.panelStatus, error.message, "status-bad");
    }
  });
  els.maxPlayerForm.addEventListener("submit", async (event) => {
    event.preventDefault();
    try {
      await saveMaxPlayers(els.maxPlayerInput.value);
      setMessage(els.panelStatus, "最大玩家数已更新", "status-ok");
    } catch (error) {
      setMessage(els.panelStatus, error.message, "status-bad");
    }
  });
  els.toggleWhitelistBtn.addEventListener("click", async () => {
    try {
      await toggleWhitelist();
    } catch (error) {
      setMessage(els.panelStatus, error.message, "status-bad");
    }
  });
  els.whitelistToggleBtn.addEventListener("click", async () => {
    try {
      await toggleWhitelist();
    } catch (error) {
      setMessage(els.panelStatus, error.message, "status-bad");
    }
  });
  els.reloadMotdBtn.addEventListener("click", async () => {
    try {
      await reloadMotd();
      setMessage(els.panelStatus, "MOTD 已重载", "status-ok");
    } catch (error) {
      setMessage(els.panelStatus, error.message, "status-bad");
    }
  });
  els.motdReloadBtn.addEventListener("click", async () => {
    try {
      await reloadMotd();
      setMessage(els.motdMessage, "已重载", "status-ok");
    } catch (error) {
      setMessage(els.motdMessage, error.message, "status-bad");
    }
  });
  els.motdPrettyBtn.addEventListener("click", () => {
    try {
      els.motdEditor.value = formatJson(JSON.parse(els.motdEditor.value || "{}"));
      setMessage(els.motdMessage, "已格式化", "status-ok");
    } catch (error) {
      setMessage(els.motdMessage, error.message, "status-bad");
    }
  });
  els.motdSaveBtn.addEventListener("click", async () => {
    try {
      await saveMotd();
    } catch (error) {
      setMessage(els.motdMessage, error.message, "status-bad");
    }
  });
  els.whiteAddForm.addEventListener("submit", async (event) => {
    event.preventDefault();
    const username = els.whiteAddInput.value.trim();
    if (!username) return;
)paneljs";
		value += R"paneljs(    try {
      await addListUser("/api/add_whitelist_user", username);
      els.whiteAddInput.value = "";
    } catch (error) {
      setMessage(els.panelStatus, error.message, "status-bad");
    }
  });
  els.blackAddForm.addEventListener("submit", async (event) => {
    event.preventDefault();
    const username = els.blackAddInput.value.trim();
    if (!username) return;
    try {
      await addListUser("/api/add_blacklist_user", username);
      els.blackAddInput.value = "";
    } catch (error) {
      setMessage(els.panelStatus, error.message, "status-bad");
    }
  });
  els.userProxyForm.addEventListener("submit", async (event) => {
    event.preventDefault();
    const username = els.proxyUsername.value.trim();
    const address = els.proxyAddress.value.trim();
    const port = Number(els.proxyPort.value);
    if (!username || !address || !port) {
      setMessage(els.panelStatus, "请填写代理必填项", "status-bad");
      return;
    }
    try {
      await setUserProxy(username, address, port);
      els.proxyUsername.value = "";
      els.proxyAddress.value = "";
      els.proxyPort.value = "";
      setMessage(els.panelStatus, "玩家代理已保存", "status-ok");
    } catch (error) {
      setMessage(els.panelStatus, error.message, "status-bad");
    }
  });

  function bootstrap() {
    if (isSessionValid()) {
      showPanel();
      refreshAll();
    } else {
      clearSession();
      showLogin();
    }
    if (state.timer) clearInterval(state.timer);
    state.timer = setInterval(() => {
      if (!isSessionValid()) {
        if (state.token) {
          clearSession();
          showLogin("会话已过期");
        }
        return;
      }
      refreshAll(true);
    }, 15000);
  }

  bootstrap();
})();)paneljs";
		return value;
	}();
	return js;
}
