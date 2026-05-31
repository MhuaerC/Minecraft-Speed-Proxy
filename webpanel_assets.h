#pragma once

#include <string>
#include <string_view>

inline constexpr std::string_view kWebPanelHtml = R"html(<!doctype html>
<html lang="zh-CN">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Minecraft 加速代理 · 管理控制台</title>
  <link rel="stylesheet" href="/panel.css">
</head>
<body>
  <div id="loginView" class="login-screen">
    <form id="loginForm" class="login-card">
      <div class="login-mark">MSP</div>
      <div class="login-brand">Minecraft 加速代理</div>
      <div class="login-sub">输入管理密码以进入控制台</div>
      <label for="passwordInput">管理密码</label>
      <input id="passwordInput" type="password" autocomplete="current-password" placeholder="管理密码">
      <button class="primary block" type="submit">登录</button>
      <div id="loginMessage" class="status-line"></div>
    </form>
  </div>

  <div id="panelView" class="app hidden">
    <aside class="sidebar">
      <div class="side-brand">
        <span class="brand-mark">MSP</span>
        <span class="brand-name">加速代理<small>管理控制台</small></span>
      </div>
      <nav class="nav" aria-label="页面导航">
        <button type="button" class="tab active" data-tab="overview">概览</button>
        <button type="button" class="tab" data-tab="servers">服务器</button>
        <button type="button" class="tab" data-tab="users">在线玩家</button>
        <button type="button" class="tab" data-tab="lists">名单</button>
        <button type="button" class="tab" data-tab="proxies">玩家代理</button>
        <button type="button" class="tab" data-tab="motd">MOTD</button>
        <button type="button" class="tab" data-tab="logs">日志</button>
        <button type="button" class="tab" data-tab="settings">设置</button>
      </nav>
      <div class="side-foot">
        <span id="connectionState" class="status-pill status-warn">未连接</span>
      </div>
    </aside>

    <main class="main">
      <header class="topbar">
        <div class="page-head">
          <h1 id="pageTitle">概览</h1>
          <div id="pageSub" class="page-sub">服务器实时状态与常用操作</div>
        </div>
        <div class="toolbar">
          <select id="activeProxySelect" class="server-select" aria-label="当前服务器"></select>
          <button id="refreshBtn" class="secondary" type="button">刷新</button>
          <button id="logoutBtn" class="ghost" type="button">退出</button>
        </div>
      </header>

      <div class="content">
        <section id="overview" class="tab-panel">
          <div class="metric-grid">
            <div class="metric"><div class="label">在线人数</div><div id="metricOnline" class="value">-</div></div>
            <div class="metric"><div class="label">最大人数</div><div id="metricMax" class="value">-</div></div>
            <div class="metric"><div class="label">白名单</div><div id="metricWhitelist" class="value">-</div></div>
            <div class="metric"><div class="label">运行时长</div><div id="metricUptime" class="value">-</div></div>
            <div class="metric"><div class="label">默认代理</div><div id="metricProxy" class="value sm long-text">-</div></div>
            <div class="metric"><div class="label">启动时间</div><div id="metricStart" class="value sm long-text">-</div></div>
          </div>
          <div class="grid-2">
            <div class="panel-box">
              <div class="card-title">常用操作</div>
              <div class="btn-row">
                <button id="toggleWhitelistBtn" class="secondary" type="button">切换白名单</button>
                <button id="reloadMotdBtn" class="secondary" type="button">重载 MOTD</button>
              </div>
              <div id="overviewMessage" class="status-line"></div>
            </div>
            <div class="panel-box">
              <div class="card-title">最大玩家数</div>
              <form id="maxPlayerForm">
                <div class="form-row">
                  <input id="maxPlayerInput" type="number" step="1" placeholder="-1">
                  <button class="primary" type="submit">保存</button>
                </div>
                <div class="field-hint">填写 -1 表示不限制人数</div>
              </form>
            </div>
          </div>
        </section>

        <section id="servers" class="tab-panel hidden">
          <div class="grid-2">
            <div class="panel-box">
              <div class="card-title">创建服务器</div>
              <form id="serverForm" class="stack-form">
                <div class="field">
                  <label for="serverName">服务器名称</label>
                  <input id="serverName" type="text" placeholder="例如：生存服">
                </div>
                <div class="field">
                  <label>本地监听</label>
                  <div class="form-row">
                    <input id="serverLocalAddress" type="text" placeholder="0.0.0.0">
                    <input id="serverLocalPort" type="number" min="1" max="65535" step="1" placeholder="本地端口">
                  </div>
                </div>
                <div class="field">
                  <label>目标服务器</label>
                  <div class="form-row">
                    <input id="serverRemoteAddress" type="text" placeholder="远程地址">
                    <input id="serverRemotePort" type="number" min="1" max="65535" step="1" placeholder="远程端口">
                  </div>
                </div>
                <div class="field">
                  <label>可选项</label>
                  <div class="form-row">
                    <input id="serverMaxPlayer" type="number" step="1" placeholder="最大人数 -1">
                    <input id="serverMotdPath" type="text" placeholder="MOTD 路径">
                  </div>
                </div>
                <button class="primary" type="submit">创建服务器</button>
              </form>
            </div>
            <div class="panel-box">
              <div class="card-title">服务器列表</div>
              <div class="table-wrap">
                <table>
                  <thead>
                    <tr><th>名称</th><th>监听地址</th><th>目标服务器</th><th>在线</th><th>操作</th></tr>
                  </thead>
                  <tbody id="serversTable"></tbody>
                </table>
              </div>
            </div>
          </div>
        </section>

        <section id="users" class="tab-panel hidden">
          <div class="panel-box">
            <div class="card-title">在线玩家</div>
            <div class="table-wrap">
              <table>
                <thead>
                  <tr><th>玩家</th><th>UUID</th><th>IP</th><th>流量</th><th>上线时间</th><th>代理</th><th>操作</th></tr>
                </thead>
                <tbody id="usersTable"></tbody>
              </table>
            </div>
          </div>
        </section>

        <section id="lists" class="tab-panel hidden">
          <div class="grid-2">
            <div class="panel-box">
              <div class="card-head">
                <div class="card-title">白名单</div>
                <button id="whitelistToggleBtn" class="secondary small" type="button">切换</button>
              </div>
              <form id="whiteAddForm" class="form-row">
                <input id="whiteAddInput" type="text" placeholder="玩家用户名">
                <button class="primary" type="submit">添加</button>
              </form>
              <div id="whiteList" class="list-stack"></div>
            </div>
            <div class="panel-box">
              <div class="card-head">
                <div class="card-title">黑名单</div>
              </div>
              <form id="blackAddForm" class="form-row">
                <input id="blackAddInput" type="text" placeholder="玩家用户名">
                <button class="primary" type="submit">添加</button>
              </form>
              <div id="blackList" class="list-stack"></div>
            </div>
          </div>
        </section>

        <section id="proxies" class="tab-panel hidden">
          <div class="grid-2">
            <div class="panel-box">
              <div class="card-title">默认代理</div>
              <div id="defaultProxyText" class="code-block long-text">-</div>
              <form id="userProxyForm" class="stack-form gap-top">
                <div class="card-title">玩家专属代理</div>
                <div class="field">
                  <label for="proxyUsername">玩家用户名</label>
                  <input id="proxyUsername" type="text" placeholder="玩家用户名">
                </div>
                <div class="field">
                  <label>代理目标</label>
                  <div class="form-row">
                    <input id="proxyAddress" type="text" placeholder="地址">
                    <input id="proxyPort" type="number" min="1" max="65535" step="1" placeholder="端口">
                  </div>
                </div>
                <button class="primary" type="submit">保存代理</button>
              </form>
            </div>
            <div class="panel-box">
              <div class="card-title">玩家代理列表</div>
              <div class="table-wrap">
                <table>
                  <thead>
                    <tr><th>玩家</th><th>目标</th><th>操作</th></tr>
                  </thead>
                  <tbody id="proxyTable"></tbody>
                </table>
              </div>
            </div>
          </div>
        </section>

        <section id="motd" class="tab-panel hidden">
          <div class="panel-box">
            <div class="card-head">
              <div class="card-title">MOTD 编辑</div>
              <div class="btn-row">
                <button id="motdPrettyBtn" class="secondary small" type="button">格式化</button>
                <button id="motdReloadBtn" class="secondary small" type="button">重载</button>
                <button id="motdSaveBtn" class="primary small" type="button">保存</button>
              </div>
            </div>
            <textarea id="motdEditor" spellcheck="false"></textarea>
            <div id="motdMessage" class="status-line"></div>
          </div>
        </section>

        <section id="logs" class="tab-panel hidden">
          <div class="panel-box">
            <div class="card-title">运行日志</div>
            <div class="table-wrap">
              <table>
                <thead>
                  <tr><th class="col-time">时间</th><th>消息</th></tr>
                </thead>
                <tbody id="logsTable"></tbody>
              </table>
            </div>
          </div>
        </section>

        <section id="settings" class="tab-panel hidden">
          <div class="grid-2">
            <div class="panel-box">
              <div class="card-title">服务器信息</div>
              <div id="serverInfo" class="info-text long-text">-</div>
            </div>
            <div class="panel-box">
              <div class="card-title">面板状态</div>
              <div id="panelStatus" class="info-text long-text">-</div>
            </div>
          </div>
        </section>
      </div>
    </main>
  </div>

  <script src="/panel.js"></script>
</body>
</html>)html";

inline constexpr std::string_view kWebPanelCss = R"css(:root {
  color-scheme: dark;
  --bg: #1b1715;
  --surface: #221d19;
  --surface-2: #2a241f;
  --field: #191512;
  --border: #39312b;
  --border-strong: #4a3f37;
  --text: #ece5dd;
  --muted: #ab9f93;
  --faint: #7d7166;
  --accent: #cc7351;
  --accent-bright: #dc8763;
  --accent-quiet: #33251e;
  --on-accent: #1b110c;
  --ok: #94ad6f;
  --warn: #d6a25a;
  --danger: #dc6c5a;
  --r: 10px;
  --r-sm: 8px;
  --shadow: 0 18px 40px rgba(0, 0, 0, 0.32);
}

* { box-sizing: border-box; }
html, body { height: 100%; }

body {
  margin: 0;
  background: var(--bg);
  color: var(--text);
  font: 14px/1.55 -apple-system, BlinkMacSystemFont, "Segoe UI", "Microsoft YaHei", system-ui, sans-serif;
  -webkit-font-smoothing: antialiased;
}

button, input, select, textarea { font: inherit; color: inherit; }
::placeholder { color: var(--faint); }
.hidden { display: none !important; }
.long-text { overflow-wrap: anywhere; word-break: break-word; }
.mono { font-family: ui-monospace, SFMono-Regular, Consolas, "Liberation Mono", monospace; }

button {
  border: 1px solid var(--border);
  background: var(--surface-2);
  color: var(--text);
  border-radius: var(--r-sm);
  padding: 9px 14px;
  cursor: pointer;
  font-weight: 500;
  transition: background-color 0.15s ease, border-color 0.15s ease, color 0.15s ease;
}
button:hover { border-color: var(--border-strong); }
button:active { transform: translateY(1px); }
button.primary { background: var(--accent); border-color: var(--accent); color: var(--on-accent); font-weight: 600; }
button.primary:hover { background: var(--accent-bright); border-color: var(--accent-bright); }
button.secondary:hover { border-color: var(--accent); color: var(--accent-bright); }
button.ghost { background: transparent; border-color: transparent; color: var(--muted); }
button.ghost:hover { color: var(--text); background: var(--surface-2); }
button.danger { background: transparent; border-color: rgba(220, 108, 90, 0.4); color: var(--danger); }
button.danger:hover { background: rgba(220, 108, 90, 0.12); border-color: var(--danger); }
button.small { padding: 6px 11px; font-size: 13px; }
button.block { width: 100%; }

input, select, textarea {
  width: 100%;
  background: var(--field);
  border: 1px solid var(--border);
  border-radius: var(--r-sm);
  padding: 9px 11px;
  transition: border-color 0.15s ease, box-shadow 0.15s ease;
}
input:focus, select:focus, textarea:focus {
  outline: none;
  border-color: var(--accent);
  box-shadow: 0 0 0 3px rgba(204, 115, 81, 0.16);
}
textarea {
  min-height: 300px;
  resize: vertical;
  font-family: ui-monospace, SFMono-Regular, Consolas, "Liberation Mono", monospace;
  line-height: 1.5;
}
label { display: block; font-size: 12px; color: var(--muted); margin-bottom: 6px; }
.field { margin-bottom: 14px; }
.field-hint { margin-top: 8px; font-size: 12px; color: var(--faint); }

.login-screen { min-height: 100vh; display: grid; place-items: center; padding: 24px; }
.login-card {
  width: min(380px, 100%);
  background: var(--surface);
  border: 1px solid var(--border);
  border-top: 3px solid var(--accent);
  border-radius: 14px;
  padding: 32px 30px 26px;
  box-shadow: var(--shadow);
}
.login-mark {
  display: inline-grid;
  place-items: center;
  width: 42px; height: 42px;
  border-radius: 12px;
  background: var(--accent);
  color: var(--on-accent);
  font-weight: 800;
  letter-spacing: 0.5px;
  margin-bottom: 18px;
}
.login-brand { font-size: 19px; font-weight: 700; }
.login-sub { color: var(--muted); margin: 5px 0 24px; font-size: 13px; }
.login-card input { margin-bottom: 18px; }

.app { display: grid; grid-template-columns: 236px 1fr; min-height: 100vh; }
.sidebar {
  background: var(--surface);
  border-right: 1px solid var(--border);
  display: flex;
  flex-direction: column;
  padding: 18px 14px;
  position: sticky;
  top: 0;
  height: 100vh;
}
.side-brand { display: flex; align-items: center; gap: 11px; padding: 4px 8px 20px; }
.brand-mark {
  display: inline-grid;
  place-items: center;
  width: 36px; height: 36px;
  border-radius: 10px;
  background: var(--accent);
  color: var(--on-accent);
  font-weight: 800;
  font-size: 13px;
  letter-spacing: 0.5px;
}
.brand-name { display: flex; flex-direction: column; font-weight: 700; font-size: 15px; line-height: 1.2; }
.brand-name small { font-weight: 500; font-size: 11px; color: var(--muted); margin-top: 3px; }
.nav { display: flex; flex-direction: column; gap: 3px; }
.nav .tab {
  position: relative;
  display: block;
  width: 100%;
  text-align: left;
  background: transparent;
  border: 0;
  border-radius: var(--r-sm);
  padding: 9px 13px;
  color: var(--muted);
  font-weight: 500;
}
.nav .tab:hover { background: var(--surface-2); color: var(--text); }
.nav .tab.active { background: var(--accent-quiet); color: var(--accent-bright); font-weight: 600; }
.nav .tab.active::before {
  content: "";
  position: absolute;
  left: -14px;
  top: 9px; bottom: 9px;
  width: 3px;
  border-radius: 0 3px 3px 0;
  background: var(--accent);
}
.side-foot { margin-top: auto; padding: 16px 6px 2px; }

.main { min-width: 0; display: flex; flex-direction: column; }
.topbar {
  display: flex;
  justify-content: space-between;
  align-items: center;
  gap: 16px;
  padding: 18px 26px;
  border-bottom: 1px solid var(--border);
  position: sticky;
  top: 0;
  background: var(--bg);
  z-index: 5;
}
.page-head h1 { margin: 0; font-size: 20px; font-weight: 700; }
.page-sub { color: var(--muted); font-size: 13px; margin-top: 3px; }
.toolbar { display: flex; align-items: center; gap: 10px; flex-wrap: wrap; }
.server-select { width: min(240px, 46vw); }
.content { padding: 24px 26px; width: 100%; max-width: 1320px; }

.status-pill {
  display: inline-flex;
  align-items: center;
  gap: 7px;
  border-radius: var(--r-sm);
  padding: 6px 11px;
  border: 1px solid var(--border);
  background: var(--surface-2);
  font-size: 12px;
  color: var(--muted);
}
.status-pill::before { content: ""; width: 7px; height: 7px; border-radius: 50%; background: var(--faint); }
.status-pill.status-ok { color: var(--ok); border-color: rgba(148, 173, 111, 0.32); background: rgba(148, 173, 111, 0.1); }
.status-pill.status-ok::before { background: var(--ok); }
.status-pill.status-warn { color: var(--warn); border-color: rgba(214, 162, 90, 0.3); background: rgba(214, 162, 90, 0.1); }
.status-pill.status-warn::before { background: var(--warn); }
.status-pill.status-bad { color: var(--danger); border-color: rgba(220, 108, 90, 0.32); background: rgba(220, 108, 90, 0.1); }
.status-pill.status-bad::before { background: var(--danger); }
.status-line { color: var(--muted); font-size: 13px; min-height: 18px; margin-top: 12px; }
.status-ok { color: var(--ok); }
.status-warn { color: var(--warn); }
.status-bad { color: var(--danger); }

.metric-grid { display: grid; gap: 14px; grid-template-columns: repeat(auto-fit, minmax(150px, 1fr)); }
.metric { background: var(--surface); border: 1px solid var(--border); border-radius: var(--r); padding: 16px; }
.metric .label { font-size: 12px; color: var(--muted); margin-bottom: 10px; }
.metric .value { font-size: 25px; font-weight: 700; font-variant-numeric: tabular-nums; line-height: 1.2; }
.metric .value.sm { font-size: 15px; font-weight: 600; }

.grid-2 { display: grid; gap: 16px; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); }
#overview .grid-2 { margin-top: 16px; }

.panel-box { background: var(--surface); border: 1px solid var(--border); border-radius: var(--r); padding: 18px; }
.card-title { font-size: 14px; font-weight: 600; color: var(--text); margin-bottom: 14px; }
.card-head { display: flex; align-items: center; justify-content: space-between; gap: 12px; margin-bottom: 14px; }
.card-head .card-title { margin-bottom: 0; }
.btn-row { display: flex; gap: 10px; flex-wrap: wrap; }
.gap-top { margin-top: 20px; }

.form-row { display: grid; grid-template-columns: repeat(auto-fit, minmax(120px, 1fr)); gap: 10px; }
.stack-form > * + * { margin-top: 14px; }
.stack-form .field { margin-bottom: 0; }

.list-stack { display: grid; gap: 8px; margin-top: 14px; }
.list-item {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 10px;
  background: var(--surface-2);
  border: 1px solid var(--border);
  border-radius: var(--r-sm);
  padding: 9px 12px;
}

.code-block {
  font-family: ui-monospace, SFMono-Regular, Consolas, "Liberation Mono", monospace;
  background: var(--field);
  border: 1px solid var(--border);
  border-radius: var(--r-sm);
  padding: 11px 13px;
  color: var(--text);
  font-size: 13px;
}
.info-text { color: var(--muted); line-height: 1.8; font-size: 13px; }

.table-wrap { overflow-x: auto; }
table { width: 100%; border-collapse: collapse; font-size: 13px; }
thead th {
  text-align: left;
  padding: 10px;
  border-bottom: 1px solid var(--border);
  color: var(--muted);
  font-size: 12px;
  font-weight: 600;
  white-space: nowrap;
}
tbody td { padding: 11px 10px; border-bottom: 1px solid var(--border); vertical-align: middle; }
tbody tr:last-child td { border-bottom: 0; }
tbody tr:hover { background: var(--surface-2); }
.note { color: var(--faint); text-align: center; padding: 22px 0 !important; }
.action-cell { white-space: nowrap; }
.action-cell button { padding: 6px 10px; font-size: 13px; }
.action-cell button + button { margin-left: 8px; }
.col-time { width: 180px; }

@media (max-width: 860px) {
  .app { grid-template-columns: 1fr; }
  .sidebar {
    position: static;
    height: auto;
    flex-direction: row;
    align-items: center;
    gap: 12px;
    overflow-x: auto;
    padding: 12px 14px;
    border-right: 0;
    border-bottom: 1px solid var(--border);
  }
  .side-brand { padding: 0; flex-shrink: 0; }
  .brand-name { display: none; }
  .nav { flex-direction: row; gap: 4px; }
  .nav .tab { white-space: nowrap; padding: 8px 12px; }
  .nav .tab.active::before { left: 8px; right: 8px; top: auto; bottom: -1px; width: auto; height: 3px; border-radius: 3px 3px 0 0; }
  .side-foot { margin: 0 0 0 auto; padding: 0; flex-shrink: 0; }
  .topbar { padding: 14px 16px; }
  .content { padding: 16px; }
}
)css";

inline auto GetWebPanelJs() -> const std::string&
{
	static const std::string js = []() {
		std::string value;
		value.reserve(28650);
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

  const tabMeta = {
    overview: ["概览", "服务器实时状态与常用操作"],
    servers: ["服务器", "创建并管理多个加速实例"],
    users: ["在线玩家", "查看并管理当前连接的玩家"],
    lists: ["名单", "白名单与黑名单管理"],
    proxies: ["玩家代理", "为指定玩家设置专属上游"],
    motd: ["MOTD", "编辑服务器列表展示信息"],
    logs: ["日志", "最近的运行日志"],
    settings: ["设置", "当前服务器与面板状态"]
  };

  function activateTab(name) {
    state.activeTab = name;
    document.querySelectorAll(".tab").forEach((btn) => {
      btn.classList.toggle("active", btn.dataset.tab === name);
    });
    document.querySelectorAll(".tab-panel").forEach((panel) => {
      panel.classList.toggle("hidden", panel.id !== name);
    });
    const meta = tabMeta[name];
    if (meta) {
      const titleEl = document.querySelector("#pageTitle");
      const subEl = document.querySelector("#pageSub");
      if (titleEl) titleEl.textContent = meta[0];
      if (subEl) subEl.textContent = meta[1];
    }
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
