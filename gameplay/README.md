# o_brother

**o_brother** 是一款仿土豆兄弟的跨平台游戏，旨在通过 **Unreal Engine 5 (UE5)** 和 **Cocos** 引擎发布到多个平台，包括桌面游戏（端游）、移动游戏（手游）以及微信小游戏。

## 目录

- [简介](#简介)
- [特性](#特性)
- [技术栈](#技术栈)
- [项目结构](#项目结构)
- [安装与构建](#安装与构建)
    - [前置条件](#前置条件)
    - [克隆仓库](#克隆仓库)
    - [构建过程](#构建过程)
- [使用说明](#使用说明)
- [开发指南](#开发指南)
- [贡献](#贡献)
- [许可证](#许可证)
- [致谢](#致谢)

## 简介

**o_brother** 是一款跨平台的多人游戏，灵感来源于经典的土豆兄弟系列。通过结合 **UE5** 和 **Cocos** 引擎，游戏能够在不同的平台上提供流畅和高质量的用户体验。项目采用 **Rust** 语言编写核心逻辑，并通过 **WebAssembly (Wasm)** 实现与前端的高效交互。

## 特性

- **跨平台支持**：同时支持桌面端、移动端和微信小游戏平台。
- **高性能**：利用 Rust 的高效性能和 WebAssembly 的跨平台能力，实现流畅的游戏体验。
- **模块化设计**：采用 ECS（Entity Component System）架构，便于扩展和维护。
- **多引擎集成**：结合 UE5 和 Cocos 引擎的优点，提供丰富的图形和交互效果。

## 技术栈

- **编程语言**：Rust
- **WebAssembly (Wasm)**：用于前后端交互和高性能计算
- **游戏引擎**：Unreal Engine 5 (UE5) 和 Cocos
- **依赖库**：
    - `wee_alloc`：轻量级分配器
    - `serde` 和 `serde_json`：序列化和反序列化
    - `hecs`：ECS 组件系统
    - `hashbrown`：高效哈希表实现

## 项目结构

```
gameplay
├── Cargo.toml
├── LICENSE
├── Makefile.toml
├── pkg
├── README.md
└── src
    ├── allocator.rs
    ├── engine.rs
    ├── lib.rs
    └── lockstep.rs
```

- **Cargo.toml**：项目的配置文件，定义了依赖和编译选项。
- **LICENSE**：项目的许可证信息。
- **Makefile.toml**：定义了构建任务，支持 Win64 和 Wasm 目标的构建与复制。
- **pkg/**：编译后的包文件目录。
- **src/**：源代码目录。
    - **allocator.rs**：自定义内存分配器配置。
    - **engine.rs**：引擎接口和日志功能实现。
    - **lib.rs**：项目的主库文件，包含核心逻辑。
    - **lockstep.rs**：锁步同步模拟器实现。

## 安装与构建

### 前置条件

确保你的开发环境中已经安装了以下工具：

- [Rust](https://www.rust-lang.org/tools/install)（版本建议：最新稳定版）
- [Cargo](https://doc.rust-lang.org/cargo/getting-started/installation.html)（Rust 的包管理工具）
- [Unreal Engine 5 (UE5)](https://www.unrealengine.com/download)（用于端游发布）
- [Cocos](https://www.cocos.com/en/)（用于手游和微信小游戏发布）
- `cp` 命令（Unix 系统自带，Windows 用户可使用 [Cygwin](https://www.cygwin.com/) 或 [Git Bash](https://gitforwindows.org/)）

### 克隆仓库

```bash
git clone https://your-repository-url/gameplay.git
cd gameplay
```

### 构建过程

项目使用 `cargo` 进行构建，并通过 `Makefile.toml` 定义了多种构建任务。

#### 构建 Win64 版本并复制 DLL

```bash
cargo install cargo-make
cargo make ue5
```

该命令将执行以下任务：
1. **build_win64**：构建 Win64 目标（生成 `gameplay.dll`）。
2. **copy_dll**：将生成的 `gameplay.dll` 复制到 UE5 插件目录。

#### 构建 WebAssembly (Wasm) 版本并复制

```bash
cargo make minigame
```

该命令将执行以下任务：
1. **build_wasm**：构建 Wasm 目标（生成 `gameplay.wasm`）。
2. **copy_wasm**：将生成的 `gameplay.wasm` 复制到微信小游戏的目录。

## 使用说明

### 运行 WebAssembly 示例

假设你有一个前端项目需要使用生成的 `gameplay.wasm`，可以参考以下步骤：

1. **准备前端环境**：

   创建一个简单的 HTML 文件，并引入 `game.js` 脚本。

   ```html
   <!DOCTYPE html>
   <html lang="en">
   <head>
       <meta charset="UTF-8">
       <meta http-equiv="X-UA-Compatible" content="IE=edge">
       <meta name="viewport" content="width=device-width, initial-scale=1.0">
       <title>o_brother</title>
   </head>
   <body>
       <script src="game.js"></script>
   </body>
   </html>
   ```

2. **配置 `game.js`**：

   确保 `game.js` 正确引用了 `gameplay.wasm`。根据你提供的代码，`game.js` 已经配置好了 WebAssembly 实例化和内存管理。

3. **启动本地服务器**：

   使用任何静态服务器工具启动本地服务器，例如：

   ```bash
   npx http-server .
   ```

4. **访问游戏**：

   在浏览器中打开 `http://localhost:8080`（端口号根据实际情况调整），查看游戏效果和日志输出。

## 开发指南

### 模块说明

- **allocator.rs**：配置全局内存分配器，使用 `wee_alloc` 作为轻量级分配器，减少 Rust WebAssembly 二进制大小。

- **engine.rs**：定义日志和错误处理接口，与前端 JavaScript 通信。包括在 Wasm 和非 Wasm 环境下的不同实现。

- **lib.rs**：项目的主库文件，包含核心功能如 `start`、`update` 和 `q_add` 等导出函数。使用 ECS 架构管理游戏实体和组件。

- **lockstep.rs**：实现了锁步同步模拟器，确保多玩家游戏中的状态同步一致。

### 调试与日志

- **日志输出**：

  使用 `engine::log` 和 `engine::error` 在控制台输出日志和错误信息，方便调试。

- **内存管理**：

  确保正确管理 WebAssembly 内存，避免内存泄漏或分离错误。参考之前的解决方案，动态访问 `memory.buffer`。

## 贡献

欢迎贡献代码！请按照以下步骤进行：

1. **Fork 仓库**。
2. **创建新分支**：

   ```bash
   git checkout -b feature/你的功能
   ```

3. **提交更改**：

   ```bash
   git commit -m "添加了某某功能"
   ```

4. **推送到分支**：

   ```bash
   git push origin feature/你的功能
   ```

5. **创建 Pull Request**。

请确保在提交之前运行所有测试，并遵循代码风格规范。

## 许可证

本项目基于 [MIT 许可证](LICENSE) 进行发布。详情请参阅 [LICENSE](LICENSE) 文件。

## 致谢

- 特别感谢 [Rust](https://www.rust-lang.org/) 社区提供的高性能编程语言。
- 感谢 [Unreal Engine](https://www.unrealengine.com/) 和 [Cocos](https://www.cocos.com/) 提供的强大游戏引擎。
- 感谢所有为本项目做出贡献的开发者和测试人员。

---