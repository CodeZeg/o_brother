# O Brother

O Brother 是一个跨平台的游戏项目，使用 Rust + UE5 + 微信小游戏 实现。项目采用 ECS 架构，通过 Rust 实现核心游戏逻辑，并分别在 UE5 和微信小游戏中渲染。

## 项目结构

```
o_brother/
├── gameplay/      # Rust 核心游戏逻辑
├── minigame/      # 微信小游戏项目 
└── ue5/           # UE5 项目
```

### gameplay

使用 Rust 实现的核心游戏逻辑模块，采用 ECS 架构:

- 使用 hecs 作为 ECS 框架
- 支持编译为 WASM 供小游戏, 以及 IOS 端使用
- 支持编译为动态库供 UE5 使用，以支持 PC 和 Android 端
- 实现了帧同步(lockstep)机制

### minigame 

微信小游戏项目:

- 加载 gameplay 编译的 WASM
- 实现游戏渲染和交互

### ue5

UE5 项目:

- 通过插件方式加载 gameplay 动态库
- 使用 Mixamo 资源实现角色动画
- 实现游戏渲染和交互

## 构建说明

1. 构建 gameplay:

```bash
# 安装
cargo install cargo-make

# 构建 WASM
cargo make minigame

# 构建动态库
cargo make ue5
```

2. 运行微信小游戏:

使用微信开发者工具打开 minigame 目录

3. 运行 UE5 项目:

使用 UE5 打开 ue5/o_bro.uproject，目前对应UE5版本号5.5，其余版本号理论都可以，但是未测试

## License

MIT License
