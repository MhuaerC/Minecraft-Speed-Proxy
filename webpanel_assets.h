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
      <div class="brand-section">
        <div class="brand">🎮 Minecraft 加速代理</div>
        <div class="subtitle">管理控制台</div>
      </div>
      <div class="toolbar">
        <select id="activeProxySelect" class="server-select" aria-label="当前服务器"></select>
        <span id="connectionState" class="status-badge status-warn">未连接</span>
        <button id="refreshBtn" class="btn-secondary" type="button">刷新</button>
        <button id="logoutBtn" class="btn-ghost" type="button">退出</button>
      </div>
    </header>

    <section id="loginView" class="section login-container">
      <form id="loginForm" class="login-box">
        <div class="login-header">
          <h1>🔐 登录</h1>
          <p>请输入管理密码</p>
        </div>
        <label for="passwordInput">密码</label>
        <input id="passwordInput" type="password" autocomplete="current-password" placeholder="请输入密码">
        <button class="btn-primary" type="submit">登录</button>
        <div id="loginMessage" class="message"></div>
      </form>
    </section>

    <section id="panelView" class="hidden">
      <nav class="tabs">
        <button type="button" class="tab active" data-tab="overview">📊 概览</button>
        <button type="button" class="tab" data-tab="servers">🖥️ 服务器</button>
        <button type="button" class="tab" data-tab="users">👥 用户</button>
        <button type="button" class="tab" data-tab="lists">📋 名单</button>
        <button type="button" class="tab" data-tab="proxies">🔗 代理</button>
        <button type="button" class="tab" data-tab="motd">💬 MOTD</button>
        <button type="button" class="tab" data-tab="logs">📝 日志</button>
        <button type="button" class="tab" data-tab="settings">⚙️ 设置</button>
      </nav>

      <section id="overview" class="section tab-content">
        <div class="stats-grid">
          <div class="stat-card">
            <div class="stat-icon">👤</div>
            <div class="stat-info">
              <div class="stat-label">在线人数</div>
              <div id="metricOnline" class="stat-value">-</div>
            </div>
          </div>
          <div class="stat-card">
            <div class="stat-icon">🎯</div>
            <div class="stat-info">
              <div class="stat-label">最大人数</div>
              <div id="metricMax" class="stat-value">-</div>
            </div>
          </div>
          <div class="stat-card">
            <div class="stat-icon">✅</div>
            <div class="stat-info">
              <div class="stat-label">白名单</div>
              <div id="metricWhitelist" class="stat-value">-</div>
            </div>
          </div>
          <div class="stat-card">
            <div class="stat-icon">🌐</div>
            <div class="stat-info">
              <div class="stat-label">默认代理</div>
              <div id="metricProxy" class="stat-value">-</div>
            </div>
          </div>
          <div class="stat-card">
            <div class="stat-icon">⏱️</div>
            <div class="stat-info">
              <div class="stat-label">运行时长</div>
              <div id="metricUptime" class="stat-value">-</div>
            </div>
          </div>
          <div class="stat-card">
            <div class="stat-icon">🚀</div>
            <div class="stat-info">
              <div class="stat-label">启动时间</div>
              <div id="metricStart" class="stat-value">-</div>
            </div>
          </div>
        </div>

        <div class="grid-2">
          <div class="card">
            <h3>快速操作</h3>
            <div class="btn-group">
              <button id="toggleWhitelistBtn" class="btn-secondary" type="button">切换白名单</button>
              <button id="reloadMotdBtn" class="btn-secondary" type="button">重载 MOTD</button>
            </div>
            <div id="overviewMessage" class="message"></div>
          </div>

          <div class="card">
            <h3>最大玩家数</h3>
            <form id="maxPlayerForm">
              <label for="maxPlayerInput">设置最大玩家数（-1 为无限制）</label>
              <div class="input-group">
                <input id="maxPlayerInput" type="number" step="1" placeholder="-1">
                <button class="btn-primary" type="submit">保存</button>
              </div>
            </form>
          </div>
        </div>
      </section>

      <section id="servers" class="section tab-content hidden">
        <div class="grid-2">
          <div class="card">
            <h3>创建新服务器</h3>
            <form id="serverForm" class="form-stack">
              <label for="serverName">服务器名称</label>
              <input id="serverName" type="text" placeholder="例如：生存服">
              
              <label>本地监听</label>
              <div class="input-group">
                <input id="serverLocalAddress" type="text" placeholder="0.0.0.0">
                <input id="serverLocalPort" type="number" min="1" max="65535" step="1" placeholder="本地端口">
              </div>
              
              <label>远程服务器</label>
              <div class="input-group">
                <input id="serverRemoteAddress" type="text" placeholder="远程地址">
                <input id="serverRemotePort" type="number" min="1" max="65535" step="1" placeholder="远程端口">
              </div>
              
              <div class="input-group">
                <input id="serverMaxPlayer" type="number" step="1" placeholder="最大玩家数 (-1)">
                <input id="serverMotdPath" type="text" placeholder="MOTD 路径">
              </div>
              
              <button class="btn-primary" type="submit">创建服务器</button>
            </form>
          </div>
          
          <div class="card">
            <h3>服务器列表</h3>
            <div class="table-container">
              <table>
                <thead>
                  <tr>
                    <th>名称</th>
                    <th>本地端口</th>
                    <th>远程地址</th>
                    <th>操作</th>
                  </tr>
                </thead>
                <tbody id="serverList"></tbody>
              </table>
            </div>
          </div>
        </div>
      </section>

      <section id="users" class="section tab-content hidden">
        <div class="card">
          <h3>在线用户</h3>
          <div class="table-container">
            <table>
              <thead>
                <tr><th>用户名</th><th>UUID</th></tr>
              </thead>
              <tbody id="userList"></tbody>
            </table>
          </div>
        </div>
      </section>

      <section id="lists" class="section tab-content hidden">
        <div class="grid-2">
          <div class="card">
            <h3>白名单管理</h3>
            <form id="whiteAddForm" class="form-inline">
              <input id="whiteAddInput" type="text" placeholder="输入用户名">
              <button class="btn-primary" type="submit">添加</button>
            </form>
            <div class="table-container">
              <table>
                <thead><tr><th>用户名</th><th>操作</th></tr></thead>
                <tbody id="whiteList"></tbody>
              </table>
            </div>
          </div>
          
          <div class="card">
            <h3>黑名单管理</h3>
            <form id="blackAddForm" class="form-inline">
              <input id="blackAddInput" type="text" placeholder="输入用户名">
              <button class="btn-primary" type="submit">添加</button>
            </form>
            <div class="table-container">
              <table>
                <thead><tr><th>用户名</th><th>操作</th></tr></thead>
                <tbody id="blackList"></tbody>
              </table>
            </div>
          </div>
        </div>
      </section>

      <section id="proxies" class="section tab-content hidden">
        <div class="grid-2">
          <div class="card">
            <h3>设置用户代理</h3>
            <form id="userProxyForm">
              <label for="proxyUsername">用户名</label>
              <input id="proxyUsername" type="text" placeholder="玩家用户名">
              <label>代理服务器</label>
              <div class="input-group">
                <input id="proxyAddress" type="text" placeholder="代理地址">
                <input id="proxyPort" type="number" min="1" max="65535" step="1" placeholder="端口">
              </div>
              <button class="btn-primary" type="submit">保存</button>
            </form>
          </div>
          
          <div class="card">
            <h3>用户代理列表</h3>
            <div class="table-container">
              <table>
                <thead><tr><th>用户名</th><th>代理地址</th><th>操作</th></tr></thead>
                <tbody id="proxyList"></tbody>
              </table>
            </div>
          </div>
        </div>
      </section>

      <section id="motd" class="section tab-content hidden">
        <div class="card">
          <h3>MOTD 编辑器</h3>
          <div class="btn-group">
            <button id="whitelistToggleBtn" class="btn-secondary" type="button">切换白名单</button>
            <button id="motdReloadBtn" class="btn-secondary" type="button">重载</button>
            <button id="motdPrettyBtn" class="btn-secondary" type="button">格式化</button>
            <button id="motdSaveBtn" class="btn-primary" type="button">保存</button>
          </div>
          <textarea id="motdEditor" rows="15" placeholder="在此编辑 MOTD JSON..."></textarea>
          <div id="motdMessage" class="message"></div>
        </div>
      </section>

      <section id="logs" class="section tab-content hidden">
        <div class="card">
          <h3>系统日志</h3>
          <div class="log-container">
            <pre id="logContent">加载中...</pre>
          </div>
        </div>
      </section>

      <section id="settings" class="section tab-content hidden">
        <div class="card">
          <h3>系统设置</h3>
          <p>更多设置功能开发中...</p>
        </div>
      </section>
    </section>

    <div id="panelStatus" class="message"></div>
  </div>
  <script src="/panel.js"></script>
</body>
</html>
)html";

inline constexpr std::string_view kWebPanelCss = R"css(
* {
  margin: 0;
  padding: 0;
  box-sizing: border-box;
}

:root {
  --bg-primary: #0f1419;
  --bg-secondary: #1a1f2e;
  --bg-tertiary: #252d3d;
  --bg-hover: #2d3548;
  --accent: #4ade80;
  --accent-dark: #22c55e;
  --accent-light: #86efac;
  --text-primary: #e5e7eb;
  --text-secondary: #9ca3af;
  --text-muted: #6b7280;
  --border: #374151;
  --danger: #ef4444;
  --warning: #f59e0b;
  --success: #10b981;
  --radius: 8px;
  --shadow: 0 4px 6px -1px rgba(0, 0, 0, 0.3);
}

body {
  font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', 'Microsoft YaHei', sans-serif;
  background: var(--bg-primary);
  color: var(--text-primary);
  line-height: 1.6;
}

.app {
  min-height: 100vh;
  display: flex;
  flex-direction: column;
}

.hidden {
  display: none !important;
}

.topbar {
  background: var(--bg-secondary);
  border-bottom: 1px solid var(--border);
  padding: 1rem 2rem;
  display: flex;
  justify-content: space-between;
  align-items: center;
  box-shadow: var(--shadow);
}

.brand-section {
  display: flex;
  flex-direction: column;
  gap: 0.25rem;
}

.brand {
  font-size: 1.5rem;
  font-weight: 700;
  color: var(--accent);
}

.subtitle {
  font-size: 0.875rem;
  color: var(--text-secondary);
}

.toolbar {
  display: flex;
  align-items: center;
  gap: 1rem;
}

.server-select {
  background: var(--bg-tertiary);
  color: var(--text-primary);
  border: 1px solid var(--border);
  padding: 0.5rem 1rem;
  border-radius: var(--radius);
  font-size: 0.875rem;
  cursor: pointer;
  min-width: 150px;
}

.status-badge {
  padding: 0.375rem 0.75rem;
  border-radius: 999px;
  font-size: 0.75rem;
  font-weight: 600;
}

.status-ok { background: var(--success); color: white; }
.status-warn { background: var(--warning); color: white; }
.status-bad { background: var(--danger); color: white; }

button {
  padding: 0.5rem 1rem;
  border: none;
  border-radius: var(--radius);
  font-size: 0.875rem;
  font-weight: 500;
  cursor: pointer;
  transition: all 0.2s;
}

.btn-primary {
  background: var(--accent);
  color: var(--bg-primary);
  font-weight: 600;
}

.btn-primary:hover {
  background: var(--accent-dark);
  transform: translateY(-1px);
}

.btn-secondary {
  background: var(--bg-tertiary);
  color: var(--text-primary);
  border: 1px solid var(--border);
}

.btn-secondary:hover {
  background: var(--bg-hover);
}

.btn-ghost {
  background: transparent;
  color: var(--text-secondary);
}

.btn-ghost:hover {
  color: var(--text-primary);
  background: var(--bg-tertiary);
}

.btn-danger {
  background: var(--danger);
  color: white;
}

.btn-danger:hover {
  background: #dc2626;
}

.login-container {
  flex: 1;
  display: flex;
  align-items: center;
  justify-content: center;
  padding: 2rem;
}

.login-box {
  background: var(--bg-secondary);
  border: 1px solid var(--border);
  border-radius: var(--radius);
  padding: 2rem;
  width: 100%;
  max-width: 400px;
  box-shadow: var(--shadow);
}

.login-header {
  text-align: center;
  margin-bottom: 1.5rem;
}

.login-header h1 {
  font-size: 1.75rem;
  margin-bottom: 0.5rem;
  color: var(--accent);
}

.login-header p {
  color: var(--text-secondary);
  font-size: 0.875rem;
}

.section {
  padding: 2rem;
}

.tabs {
  display: flex;
  gap: 0.5rem;
  background: var(--bg-secondary);
  padding: 1rem 2rem;
  border-bottom: 1px solid var(--border);
  overflow-x: auto;
}

.tab {
  padding: 0.75rem 1.5rem;
  background: transparent;
  color: var(--text-secondary);
  border: none;
  border-radius: var(--radius);
  font-size: 0.875rem;
  white-space: nowrap;
}

.tab:hover {
  background: var(--bg-tertiary);
  color: var(--text-primary);
}

.tab.active {
  background: var(--accent);
  color: var(--bg-primary);
  font-weight: 600;
}

.tab-content {
  display: none;
}

.tab-content:not(.hidden) {
  display: block;
}

.card {
  background: var(--bg-secondary);
  border: 1px solid var(--border);
  border-radius: var(--radius);
  padding: 1.5rem;
  box-shadow: var(--shadow);
}

.card h3 {
  color: var(--accent);
  margin-bottom: 1rem;
  font-size: 1.125rem;
}

.grid-2 {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(400px, 1fr));
  gap: 1.5rem;
  margin-top: 1.5rem;
}

.stats-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
  gap: 1rem;
  margin-bottom: 1.5rem;
}

.stat-card {
  background: var(--bg-secondary);
  border: 1px solid var(--border);
  border-radius: var(--radius);
  padding: 1.25rem;
  display: flex;
  align-items: center;
  gap: 1rem;
}

.stat-icon {
  font-size: 2rem;
  line-height: 1;
}

.stat-info {
  flex: 1;
}

.stat-label {
  font-size: 0.75rem;
  color: var(--text-secondary);
  text-transform: uppercase;
  letter-spacing: 0.05em;
}

.stat-value {
  font-size: 1.5rem;
  font-weight: 700;
  color: var(--text-primary);
  margin-top: 0.25rem;
}

label {
  display: block;
  font-size: 0.875rem;
  font-weight: 500;
  color: var(--text-secondary);
  margin-bottom: 0.5rem;
}

input, textarea, select {
  width: 100%;
  background: var(--bg-tertiary);
  border: 1px solid var(--border);
  color: var(--text-primary);
  padding: 0.625rem;
  border-radius: var(--radius);
  font-size: 0.875rem;
  margin-bottom: 1rem;
}

input:focus, textarea:focus, select:focus {
  outline: none;
  border-color: var(--accent);
}

textarea {
  font-family: 'Consolas', 'Monaco', monospace;
  resize: vertical;
  min-height: 200px;
}

.input-group {
  display: flex;
  gap: 0.5rem;
  margin-bottom: 1rem;
}

.input-group input {
  margin-bottom: 0;
}

.form-stack label {
  margin-top: 0.5rem;
}

.form-inline {
  display: flex;
  gap: 0.5rem;
  margin-bottom: 1rem;
}

.form-inline input {
  flex: 1;
  margin-bottom: 0;
}

.btn-group {
  display: flex;
  gap: 0.5rem;
  margin-bottom: 1rem;
}

.table-container {
  overflow-x: auto;
  border: 1px solid var(--border);
  border-radius: var(--radius);
}

table {
  width: 100%;
  border-collapse: collapse;
}

thead {
  background: var(--bg-tertiary);
}

th {
  padding: 0.75rem;
  text-align: left;
  font-size: 0.75rem;
  font-weight: 600;
  color: var(--text-secondary);
  text-transform: uppercase;
  letter-spacing: 0.05em;
  border-bottom: 1px solid var(--border);
}

td {
  padding: 0.75rem;
  border-bottom: 1px solid var(--border);
  font-size: 0.875rem;
}

tbody tr:hover {
  background: var(--bg-tertiary);
}

tbody tr:last-child td {
  border-bottom: none;
}

.log-container {
  background: var(--bg-primary);
  border: 1px solid var(--border);
  border-radius: var(--radius);
  padding: 1rem;
  max-height: 600px;
  overflow-y: auto;
}

.log-container pre {
  font-family: 'Consolas', 'Monaco', monospace;
  font-size: 0.75rem;
  line-height: 1.5;
  color: var(--text-secondary);
  white-space: pre-wrap;
  word-wrap: break-word;
}

.message {
  padding: 0.75rem;
  border-radius: var(--radius);
  font-size: 0.875rem;
  margin-top: 1rem;
}

.message:empty {
  display: none;
}

.message.status-ok {
  background: rgba(16, 185, 129, 0.1);
  border: 1px solid var(--success);
  color: var(--success);
}

.message.status-bad {
  background: rgba(239, 68, 68, 0.1);
  border: 1px solid var(--danger);
  color: var(--danger);
}

@media (max-width: 768px) {
  .topbar { flex-direction: column; gap: 1rem; }
  .grid-2 { grid-template-columns: 1fr; }
  .stats-grid { grid-template-columns: 1fr; }
}
)css";

inline std::string GetWebPanelJs() {
  std::string js = R"paneljs((function() {
  'use strict';
  
  const state = {
    token: sessionStorage.getItem('token') || '',
    activeProxyId: '',
    timer: null
  };

  const els = {
    loginView: document.getElementById('loginView'),
    panelView: document.getElementById('panelView'),
    loginForm: document.getElementById('loginForm'),
    passwordInput: document.getElementById('passwordInput'),
    loginMessage: document.getElementById('loginMessage'),
    logoutBtn: document.getElementById('logoutBtn'),
    refreshBtn: document.getElementById('refreshBtn'),
    connectionState: document.getElementById('connectionState'),
    activeProxySelect: document.getElementById('activeProxySelect'),
    panelStatus: document.getElementById('panelStatus'),
    tabs: document.querySelectorAll('.tab'),
    tabPanels: document.querySelectorAll('.tab-content'),
    metricOnline: document.getElementById('metricOnline'),
    metricMax: document.getElementById('metricMax'),
    metricWhitelist: document.getElementById('metricWhitelist'),
    metricProxy: document.getElementById('metricProxy'),
    metricUptime: document.getElementById('metricUptime'),
    metricStart: document.getElementById('metricStart'),
    toggleWhitelistBtn: document.getElementById('toggleWhitelistBtn'),
    reloadMotdBtn: document.getElementById('reloadMotdBtn'),
    maxPlayerForm: document.getElementById('maxPlayerForm'),
    maxPlayerInput: document.getElementById('maxPlayerInput'),
    overviewMessage: document.getElementById('overviewMessage'),
    serverForm: document.getElementById('serverForm'),
    serverName: document.getElementById('serverName'),
    serverLocalAddress: document.getElementById('serverLocalAddress'),
    serverLocalPort: document.getElementById('serverLocalPort'),
    serverRemoteAddress: document.getElementById('serverRemoteAddress'),
    serverRemotePort: document.getElementById('serverRemotePort'),
    serverMaxPlayer: document.getElementById('serverMaxPlayer'),
    serverMotdPath: document.getElementById('serverMotdPath'),
    serverList: document.getElementById('serverList'),
    userList: document.getElementById('userList'),
    whiteAddForm: document.getElementById('whiteAddForm'),
    whiteAddInput: document.getElementById('whiteAddInput'),
    whiteList: document.getElementById('whiteList'),
    blackAddForm: document.getElementById('blackAddForm'),
    blackAddInput: document.getElementById('blackAddInput'),
    blackList: document.getElementById('blackList'),
    userProxyForm: document.getElementById('userProxyForm'),
    proxyUsername: document.getElementById('proxyUsername'),
    proxyAddress: document.getElementById('proxyAddress'),
    proxyPort: document.getElementById('proxyPort'),
    proxyList: document.getElementById('proxyList'),
    whitelistToggleBtn: document.getElementById('whitelistToggleBtn'),
    motdReloadBtn: document.getElementById('motdReloadBtn'),
    motdPrettyBtn: document.getElementById('motdPrettyBtn'),
    motdSaveBtn: document.getElementById('motdSaveBtn'),
    motdEditor: document.getElementById('motdEditor'),
    motdMessage: document.getElementById('motdMessage'),
    logContent: document.getElementById('logContent')
  };

  function setMessage(el, msg, cls) {
    if (!el) return;
    el.textContent = msg;
    el.className = 'message ' + (cls || '');
  }

  function clearSession() {
    state.token = '';
    sessionStorage.removeItem('token');
  }

  function isSessionValid() {
    return !!state.token;
  }

  function showLogin(msg) {
    els.loginView.classList.remove('hidden');
    els.panelView.classList.add('hidden');
    if (msg) setMessage(els.loginMessage, msg, 'status-bad');
  }

  function showPanel() {
    els.loginView.classList.add('hidden');
    els.panelView.classList.remove('hidden');
    els.connectionState.textContent = '已连接';
    els.connectionState.className = 'status-badge status-ok';
  }

  async function apiRequest(url, options = {}) {
    const headers = { 'Content-Type': 'application/json' };
    if (state.token) headers['Authorization'] = 'Bearer ' + state.token;
    if (state.activeProxyId) headers['X-Proxy-ID'] = state.activeProxyId;
    
    const response = await fetch(url, { ...options, headers });
    if (response.status === 401) {
      clearSession();
      showLogin('会话已过期，请重新登录');
      throw new Error('未授权');
    }
    if (!response.ok) {
      const text = await response.text();
      throw new Error(text || '请求失败');
    }
    const contentType = response.headers.get('content-type');
    if (contentType && contentType.includes('application/json')) {
      return response.json();
    }
    return response.text();
  }

  async function login(password) {
    const data = await apiRequest('/api/login', {
      method: 'POST',
      body: JSON.stringify({ password })
    });
    state.token = data.token;
    sessionStorage.setItem('token', data.token);
  }

  async function getProxyServers() {
    return apiRequest('/api/get_proxy_servers');
  }

  async function createProxyServer(payload) {
    return apiRequest('/api/create_proxy_server', {
      method: 'POST',
      body: JSON.stringify(payload)
    });
  }

  async function removeProxyServer(proxyId) {
    return apiRequest('/api/remove_proxy_server', {
      method: 'POST',
      body: JSON.stringify({ proxy_id: proxyId })
    });
  }

  async function getStatus() {
    return apiRequest('/api/get_status');
  }

  async function getOnlineUsers() {
    return apiRequest('/api/get_online_users');
  }

  async function toggleWhitelist() {
    return apiRequest('/api/toggle_whitelist', { method: 'POST' });
  }

  async function reloadMotd() {
    return apiRequest('/api/reload_motd', { method: 'POST' });
  }

  async function saveMaxPlayers(max) {
    return apiRequest('/api/save_max_players', {
      method: 'POST',
      body: JSON.stringify({ max_players: Number(max) })
    });
  }

  async function getWhitelist() {
    return apiRequest('/api/get_whitelist');
  }

  async function getBlacklist() {
    return apiRequest('/api/get_blacklist');
  }

  async function addListUser(endpoint, username) {
    return apiRequest(endpoint, {
      method: 'POST',
      body: JSON.stringify({ username })
    });
  }

  async function removeListUser(endpoint, username) {
    return apiRequest(endpoint, {
      method: 'POST',
      body: JSON.stringify({ username })
    });
  }

  async function getUserProxies() {
    return apiRequest('/api/get_user_proxies');
  }

  async function setUserProxy(username, address, port) {
    return apiRequest('/api/set_user_proxy', {
      method: 'POST',
      body: JSON.stringify({ username, address, port })
    });
  }

  async function removeUserProxy(username) {
    return apiRequest('/api/remove_user_proxy', {
      method: 'POST',
      body: JSON.stringify({ username })
    });
  }

  async function getMotd() {
    return apiRequest('/api/get_motd');
  }

  async function saveMotd() {
    const content = els.motdEditor.value;
    await apiRequest('/api/save_motd', {
      method: 'POST',
      body: JSON.stringify({ content })
    });
    setMessage(els.motdMessage, '保存成功', 'status-ok');
  }

  async function getLogs() {
    return apiRequest('/api/get_logs');
  }

  function formatJson(obj) {
    return JSON.stringify(obj, null, 2);
  }

  function formatUptime(seconds) {
    const d = Math.floor(seconds / 86400);
    const h = Math.floor((seconds % 86400) / 3600);
    const m = Math.floor((seconds % 3600) / 60);
    const s = Math.floor(seconds % 60);
    const parts = [];
    if (d > 0) parts.push(d + '天');
    if (h > 0) parts.push(h + '小时');
    if (m > 0) parts.push(m + '分钟');
    if (s > 0 || parts.length === 0) parts.push(s + '秒');
    return parts.join(' ');
  }

  function switchTab(tabName) {
    els.tabs.forEach(t => t.classList.remove('active'));
    els.tabPanels.forEach(p => p.classList.add('hidden'));
    const activeTab = document.querySelector([data-tab=""]);
    const activePanel = document.getElementById(tabName);
    if (activeTab) activeTab.classList.add('active');
    if (activePanel) activePanel.classList.remove('hidden');
  }

  async function refreshProxyServers() {
    try {
      const servers = await getProxyServers();
      els.activeProxySelect.innerHTML = '';
      servers.forEach(s => {
        const opt = document.createElement('option');
        opt.value = s.proxy_id;
        opt.textContent = s.name || s.proxy_id;
        els.activeProxySelect.appendChild(opt);
      });
      if (servers.length > 0 && !state.activeProxyId) {
        state.activeProxyId = servers[0].proxy_id;
        els.activeProxySelect.value = state.activeProxyId;
      }
      els.serverList.innerHTML = '';
      servers.forEach(s => {
        const row = document.createElement('tr');
        row.innerHTML = 
          <td></td>
          <td></td>
          <td>:</td>
          <td><button class="btn-danger btn-sm" data-id="">删除</button></td>
        ;
        row.querySelector('button').addEventListener('click', async (e) => {
          const id = e.target.dataset.id;
          if (confirm('确定删除此服务器？')) {
            await removeProxyServer(id);
            refreshProxyServers();
          }
        });
        els.serverList.appendChild(row);
      });
    } catch (err) {
      console.error(err);
    }
  }

  async function refreshStatus() {
    try {
      const status = await getStatus();
      els.metricOnline.textContent = status.online_count || 0;
      els.metricMax.textContent = status.max_players || '-1';
      els.metricWhitelist.textContent = status.whitelist_enabled ? '开启' : '关闭';
      els.metricProxy.textContent = status.default_proxy || '-';
      els.metricUptime.textContent = formatUptime(status.uptime || 0);
      els.metricStart.textContent = status.start_time || '-';
      els.maxPlayerInput.value = status.max_players || -1;
    } catch (err) {
      console.error(err);
    }
  }

  async function refreshUsers() {
    try {
      const users = await getOnlineUsers();
      els.userList.innerHTML = '';
      users.forEach(u => {
        const row = document.createElement('tr');
        row.innerHTML = <td></td><td></td>;
        els.userList.appendChild(row);
      });
    } catch (err) {
      console.error(err);
    }
  }

  async function refreshWhitelist() {
    try {
      const list = await getWhitelist();
      els.whiteList.innerHTML = '';
      list.forEach(username => {
        const row = document.createElement('tr');
        row.innerHTML = 
          <td></td>
          <td><button class="btn-danger btn-sm" data-name="">移除</button></td>
        ;
        row.querySelector('button').addEventListener('click', async (e) => {
          const name = e.target.dataset.name;
          await removeListUser('/api/remove_whitelist_user', name);
          refreshWhitelist();
        });
        els.whiteList.appendChild(row);
      });
    } catch (err) {
      console.error(err);
    }
  }

  async function refreshBlacklist() {
    try {
      const list = await getBlacklist();
      els.blackList.innerHTML = '';
      list.forEach(username => {
        const row = document.createElement('tr');
        row.innerHTML = 
          <td></td>
          <td><button class="btn-danger btn-sm" data-name="">移除</button></td>
        ;
        row.querySelector('button').addEventListener('click', async (e) => {
          const name = e.target.dataset.name;
          await removeListUser('/api/remove_blacklist_user', name);
          refreshBlacklist();
        });
        els.blackList.appendChild(row);
      });
    } catch (err) {
      console.error(err);
    }
  }

  async function refreshProxies() {
    try {
      const proxies = await getUserProxies();
      els.proxyList.innerHTML = '';
      proxies.forEach(p => {
        const row = document.createElement('tr');
        row.innerHTML = 
          <td></td>
          <td>:</td>
          <td><button class="btn-danger btn-sm" data-name="">删除</button></td>
        ;
        row.querySelector('button').addEventListener('click', async (e) => {
          const name = e.target.dataset.name;
          await removeUserProxy(name);
          refreshProxies();
        });
        els.proxyList.appendChild(row);
      });
    } catch (err) {
      console.error(err);
    }
  }

  async function refreshMotd() {
    try {
      const motd = await getMotd();
      els.motdEditor.value = motd;
    } catch (err) {
      console.error(err);
    }
  }

  async function refreshLogs() {
    try {
      const logs = await getLogs();
      els.logContent.textContent = logs || '暂无日志';
    } catch (err) {
      els.logContent.textContent = '加载日志失败';
    }
  }

  async function refreshAll(silent = false) {
    try {
      await refreshProxyServers();
      await refreshStatus();
      await refreshUsers();
      await refreshWhitelist();
      await refreshBlacklist();
      await refreshProxies();
      await refreshMotd();
      await refreshLogs();
    } catch (err) {
      if (!silent) {
        setMessage(els.panelStatus, '刷新失败: ' + err.message, 'status-bad');
      }
    }
  }

  els.loginForm.addEventListener('submit', async (e) => {
    e.preventDefault();
    const password = els.passwordInput.value;
    try {
      await login(password);
      showPanel();
      refreshAll();
      els.passwordInput.value = '';
    } catch (err) {
      setMessage(els.loginMessage, '登录失败: ' + err.message, 'status-bad');
    }
  });

  els.logoutBtn.addEventListener('click', () => {
    clearSession();
    showLogin();
  });

  els.refreshBtn.addEventListener('click', () => refreshAll());

  els.activeProxySelect.addEventListener('change', (e) => {
    state.activeProxyId = e.target.value;
    refreshAll();
  });

  els.tabs.forEach(tab => {
    tab.addEventListener('click', () => {
      switchTab(tab.dataset.tab);
    });
  });

  els.serverForm.addEventListener('submit', async (e) => {
    e.preventDefault();
    const payload = {
      name: els.serverName.value.trim(),
      local_address: els.serverLocalAddress.value.trim() || '0.0.0.0',
      local_port: Number(els.serverLocalPort.value),
      remote_address: els.serverRemoteAddress.value.trim(),
      remote_port: Number(els.serverRemotePort.value || 25565),
      max_player: Number(els.serverMaxPlayer.value || -1),
      motd_path: els.serverMotdPath.value.trim()
    };
    if (!payload.local_port || !payload.remote_address || !payload.remote_port) {
      setMessage(els.panelStatus, '请填写必填字段', 'status-bad');
      return;
    }
    try {
      await createProxyServer(payload);
      els.serverName.value = '';
      els.serverLocalAddress.value = '';
      els.serverLocalPort.value = '';
      els.serverRemoteAddress.value = '';
      els.serverRemotePort.value = '';
      els.serverMaxPlayer.value = '';
      els.serverMotdPath.value = '';
      setMessage(els.panelStatus, '服务器创建成功', 'status-ok');
      refreshProxyServers();
    } catch (err) {
      setMessage(els.panelStatus, err.message, 'status-bad');
    }
  });

  els.maxPlayerForm.addEventListener('submit', async (e) => {
    e.preventDefault();
    try {
      await saveMaxPlayers(els.maxPlayerInput.value);
      setMessage(els.panelStatus, '最大玩家数已更新', 'status-ok');
    } catch (err) {
      setMessage(els.panelStatus, err.message, 'status-bad');
    }
  });

  els.toggleWhitelistBtn.addEventListener('click', async () => {
    try {
      await toggleWhitelist();
      refreshStatus();
    } catch (err) {
      setMessage(els.panelStatus, err.message, 'status-bad');
    }
  });

  els.whitelistToggleBtn.addEventListener('click', async () => {
    try {
      await toggleWhitelist();
      refreshStatus();
    } catch (err) {
      setMessage(els.panelStatus, err.message, 'status-bad');
    }
  });

  els.reloadMotdBtn.addEventListener('click', async () => {
    try {
      await reloadMotd();
      setMessage(els.panelStatus, 'MOTD 已重载', 'status-ok');
    } catch (err) {
      setMessage(els.panelStatus, err.message, 'status-bad');
    }
  });

  els.motdReloadBtn.addEventListener('click', async () => {
    try {
      await reloadMotd();
      setMessage(els.motdMessage, '已重载', 'status-ok');
    } catch (err) {
      setMessage(els.motdMessage, err.message, 'status-bad');
    }
  });

  els.motdPrettyBtn.addEventListener('click', () => {
    try {
      els.motdEditor.value = formatJson(JSON.parse(els.motdEditor.value || '{}'));
      setMessage(els.motdMessage, '已格式化', 'status-ok');
    } catch (err) {
      setMessage(els.motdMessage, err.message, 'status-bad');
    }
  });

  els.motdSaveBtn.addEventListener('click', async () => {
    try {
      await saveMotd();
    } catch (err) {
      setMessage(els.motdMessage, err.message, 'status-bad');
    }
  });

  els.whiteAddForm.addEventListener('submit', async (e) => {
    e.preventDefault();
    const username = els.whiteAddInput.value.trim();
    if (!username) return;
    try {
      await addListUser('/api/add_whitelist_user', username);
      els.whiteAddInput.value = '';
      refreshWhitelist();
    } catch (err) {
      setMessage(els.panelStatus, err.message, 'status-bad');
    }
  });

  els.blackAddForm.addEventListener('submit', async (e) => {
    e.preventDefault();
    const username = els.blackAddInput.value.trim();
    if (!username) return;
    try {
      await addListUser('/api/add_blacklist_user', username);
      els.blackAddInput.value = '';
      refreshBlacklist();
    } catch (err) {
      setMessage(els.panelStatus, err.message, 'status-bad');
    }
  });

  els.userProxyForm.addEventListener('submit', async (e) => {
    e.preventDefault();
    const username = els.proxyUsername.value.trim();
    const address = els.proxyAddress.value.trim();
    const port = Number(els.proxyPort.value);
    if (!username || !address || !port) {
      setMessage(els.panelStatus, '请填写所有字段', 'status-bad');
      return;
    }
    try {
      await setUserProxy(username, address, port);
      els.proxyUsername.value = '';
      els.proxyAddress.value = '';
      els.proxyPort.value = '';
      setMessage(els.panelStatus, '用户代理已保存', 'status-ok');
      refreshProxies();
    } catch (err) {
      setMessage(els.panelStatus, err.message, 'status-bad');
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
          showLogin('会话已过期');
        }
        return;
      }
      refreshAll(true);
    }, 15000);
  }

  bootstrap();
})();
)paneljs";
  return js;
}
