# 下载wasm-micro-runtime库

# 编译wasm-micro-runtime库
## 依赖安装：
* msvc
进入https://visualstudio.microsoft.com/zh-hans/downloads/，选择社区版下载Visual Studio 2022生成工具，下载完成点击安装，安装的组件按需选择，但是最重要的是在单个组件里把msvc勾选上

具体可参考博客：https://blog.csdn.net/b1049112625/article/details/134524652（安装好就可以了，打开powershell就有编译环境了）
*cmake
进入cmake官网，貌似可以下载msi版本就安装上了

## 编译：
### 编译wamr静态和动态库
wamr库用于ue里面调用，初始化wasm&aot虚拟机
将wasm-micro-runtime库放在某个位置，修改CMakeLists.txt里库WAMR_ROOT_DIR变量引用位置，windows下点击build.bat就可以

### 编译wamrc编译器
wamrc编译器是将wasm文件编译成aot文件的工具，不同平台都需要编译自己的aot文件，否则编译后交叉使用ue会崩溃

``` 
参照https://github.com/bytecodealliance/wasm-micro-runtime/blob/main/wamr-compiler/README.md

windows：
cd wamr-compiler
python build_llvm.py
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

# 依赖的编译产物说明
* `Plugins/oz_lib/ThirdParty/wamr/Win64/lib/{libiwasm.dll,libiwasm.lib}`放置的就是ue项目启动会加载的wasm虚拟机库文件，libiwasm.dll会自动拷贝到`Plugins/oz_lib/Binaries`目录下
* `Plugins/oz_lib/ThirdParty/wamr/Win64/Include`目录放置的wasm虚拟机引用头文件
* `Plugins/oz_lib/ThirdParty/Win64/{gameplay.wasm,gameplay.aot}`就是rust编写的gameplay逻辑库文件