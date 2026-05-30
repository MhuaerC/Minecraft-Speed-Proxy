[![Build and release](https://github.com/AllesUgo/Minecraft-Speed-Proxy/actions/workflows/release.yaml/badge.svg)](https://github.com/AllesUgo/Minecraft-Speed-Proxy/actions/workflows/release.yaml)
![GitHub code size in bytes](https://img.shields.io/github/languages/code-size/AllesUgo/Minecraft-Speed-Proxy)
![GitHub all releases](https://img.shields.io/github/downloads/AllesUgo/Minecraft-Speed-Proxy/total)
![GitHub](https://img.shields.io/github/license/AllesUgo/Minecraft-Speed-Proxy)
![GitHub release (latest SemVer)](https://img.shields.io/github/v/release/AllesUgo/Minecraft-Speed-Proxy)

# Minecraft-Speed-Proxy

Minecraft加速IP程序，支持代理Minecraft服务器，拥有白名单、用户控制、流量展示、MOTD自定义等功能，支持IPv6与Forge客户端。允许通过Web API控制服务器。

新版采用C++开发，内存占用低，崩溃率和内存泄漏风险大幅降低，结构更优。使用异步IO大幅提高吞吐量。支持跨平台编译（Windows、Linux）。如需旧版（C语言），请下载v3.0.0以前版本或切换到c-releases分支。

---

## 获取方式

### Docker Compose 部署（推荐）
仓库根目录已经放好了 `docker-compose.yml`，默认直接拉取 GHCR 镜像运行，不在本地编译。

第一次启动前，先复制示例配置，再修改项目根目录的 `config.json`：
```bash
cp config.json.example config.json
mkdir -p data
```

至少需要修改：
```json
"WebPanelPassword": "change-me"
```

启动：
```bash
docker compose pull
docker compose up -d
```

查看日志：
```bash
docker compose logs -f
```

停止：
```bash
docker compose down
```

启动后，默认加速服务器端口是 `25565`，网页管理面板默认访问：
```text
http://服务器IP:20220/
```

多加速服务器模式下，Compose 使用 `network_mode: host`，网页里新建的本地监听端口会直接监听到宿主机上，不需要每新增一个服务器就改一次端口映射。
> `network_mode: host` 主要适合 Linux Docker 主机。如果使用不支持 host 网络的 Docker Desktop，需要改回 `ports` 并提前映射一个端口范围。

### 发行版下载
1. 前往 [Release](https://github.com/AllesUgo/Minecraft-Speed-Proxy/releases/latest) 下载最新版本
2. 解压文件
   ```bash
   tar -zxvf <下载的压缩包>
   ```

### 源码编译（Linux）
>此处以试用Ubuntu为例，其他Linux发行版请自行安装相应依赖。
1. 克隆仓库
   ```bash
   sudo apt update
   sudo apt install -y git
   git clone https://github.com/AllesUgo/Minecraft-Speed-Proxy.git
   ```
2. 安装编译工具
   ```bash
   sudo apt install -y g++ cmake make
   ```
3. 编译项目
   ```bash
   cd Minecraft-Speed-Proxy
   cmake --preset=linux-release
   cd out/build/linux-release
   cmake --build .
   ```

### Windows
>编译环境较大，建议直接下载发行版。
1. 安装 [Visual Studio 2022](https://visualstudio.microsoft.com/zh-hans/downloads/)（需包含C++组件）
2. 打开Visual Studio后选择克隆存储库，并填入仓库地址克隆`https://github.com/AllesUgo/Minecraft-Speed-Proxy.git`
3. 完成后直接编译运行
---

## 使用方法

### 启动
Linux需先赋予执行权限(*一般已拥有*)：
```bash
chmod +x minecraftspeedproxy
```

在程序目录下运行：
```bash
./minecraftspeedproxy -h
```

基本命令：
```bash
./minecraftspeedproxy <服务器地址> <服务器端口> <本地端口>
```
示例：
```bash
./minecraftspeedproxy mc.hypixel.net 25565 25565
```

### 参数说明
- 必选参数：`服务器地址` `服务器端口` `本地端口`
- 更多参数：`-h` 查看帮助
- 运行后输入 `help` 获取命令支持

---

## 配置文件说明

支持命令行参数或配置文件启动。
生成配置文件：
```bash
./minecraftspeedproxy -a <配置文件路径>
```
使用配置文件启动：
```bash
./minecraftspeedproxy -c <配置文件路径>
```
>配置文件路径需要包含文件名，如`./config.json`。

仓库里的 `config.json.example` 是部署示例。实际部署时先复制一份 `config.json`，再修改自己的面板密码和默认服务器：
```bash
cp config.json.example config.json
```

示例配置内容如下：
```json
{
	"LocalAddress": "0.0.0.0",
	"LocalPort": 25565,
	"Address": "mc.hypixel.net",
	"RemotePort": 25565,
	"MaxPlayer": -1,
	"MotdPath": "",
	"DefaultEnableWhitelist": true,
	"WhiteBlcakListPath": "./data/WhiteBlackList.json",
	"ProxyListPath": "./data/proxies.json",
	"AllowInput": false,
	"ShowOnlinePlayerNumber": true,
	"LogDir": "./data/logs",
	"ShowLogLevel": 0,
	"SaveLogLevel": 0,
	"Version": "1.1",
    "WebPanelEnable": true,
	"WebPanelAddress": "0.0.0.0",
	"WebPanelPort": 20220,
	"WebPanelPassword": "change-me"
}
```

| 键名 | 类型 | 说明 |
|---|---|---|
| Version | 字符串 | 配置文件版本号 |
| LocalAddress | 字符串 | 本机地址（如`0.0.0.0`或`::`） |
| LocalPort | 整数 | 本机端口 |
| Address | 字符串 | 远程服务器地址（域名或IP） |
| RemotePort | 整数 | 远程服务器端口 |
| MaxPlayer | 整数 | 最大玩家数，-1不限制 |
| MotdPath | 字符串 | motd文件路径，空则默认 |
| DefaultEnableWhitelist | 布尔 | 是否默认启用白名单 |
| WhiteBlcakListPath | 字符串 | 白/黑名单文件路径 |
| ProxyListPath | 字符串 | 多个加速服务器配置保存路径 |
| AllowInput | 布尔 | 是否允许输入命令 |
| ShowOnlinePlayerNumber | 布尔 | 是否显示在线玩家数（暂未实现） |
| LogDir | 字符串 | 日志目录 |
| ShowLogLevel | 整数 | 显示日志等级 |
| SaveLogLevel | 整数 | 保存日志等级 |
| WebPanelEnable | 布尔 | 是否启用网页管理面板 |
| WebPanelAddress | 字符串 | 网页管理面板监听地址 |
| WebPanelPort | 整数 | 网页管理面板监听端口 |
| WebPanelPassword | 字符串 | 网页管理面板登录密码 |

---

## 网页管理面板

网页管理面板允许通过浏览器控制服务器，包含控制台全部功能及扩展。
默认启用，可在配置文件中设置`WebPanelEnable`为`false`关闭。
接口文档详见 [WebAPI.md](WebAPI.md)。
内置网页管理面板可直接访问 `http://<WebPanelAddress>:<WebPanelPort>/`。
网页管理面板支持创建多个加速服务器，每个加速服务器会启动一个独立监听端口，服务列表会保存到 `ProxyListPath` 指定的 JSON 文件。
> [!WARNING]
> 你的密码将被不加密传输，请慎重考虑面板访问范围及网络环境安全性。可以使用反向代理用HTTPS提高安全性。
---

## MOTD自定义

motd文件为JSON格式文本，可通过配置文件指定路径。
示例：
```json
{
    "version": {"name": "1.8.7", "protocol": 47},
    "players": {"max": 100, "online": 5, "sample": [{"name": "thinkofdeath", "id": "4566e69f-c907-48ee-8d71-d7ba5aa00d20"}]},
    "description": {"text": "Hello world"},
    "favicon": "data:image/png;base64,<data>"
}
```
字段可为空，服务器将自动填充默认值。  
获取现有服务器motd数据：
```bash
./minecraftspeedproxy --get-motd
```

---

## 二次开发

详见 [二次开发](SecondaryDevelopment.md)。
