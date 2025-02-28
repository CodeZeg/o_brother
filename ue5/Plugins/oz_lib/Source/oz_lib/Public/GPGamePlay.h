#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include <wasm_export.h>
#include <stdlib.h>
#include <string.h>
#include "render_data_generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGP, Log, All);

// fixme：考虑初始化了wasm虚拟机之后，创建多个模块实例（module_instance），并发调用时可以负载均衡到
// 各个实例上，每个实例做互斥锁保护

class GPGamePlayModule
{
public:
	GPGamePlayModule(FString, uint32_t, uint32_t);
	GPGamePlayModule& Load();
	static void Call_Start();
	static uint8* Call_Update(const flatbuffers::FlatBufferBuilder& InputBuilder);
private:
	inline static FString FilePath;
	inline static uint32 StackSize = 8092;
	inline static uint32 HeapSize = 8092;
	inline static bool IsAotFileModule;
	inline static wasm_module_inst_t ModuleInstance = NULL;
	inline static wasm_exec_env_t ExecEnv = NULL;
	inline static wasm_function_inst_t WasmFunStart;
	inline static wasm_function_inst_t WasmFunUpdate;
};

class GPGamePlayMgrSingleton {
public:
	static GPGamePlayMgrSingleton& GetInstance() {
		return instance;
	}

	static void Init();
	static void Release();
	static void LoadModule(FString, uint32_t, uint32_t);
	static GPGamePlayModule* GetLoadedModule();

	GPGamePlayMgrSingleton(const GPGamePlayMgrSingleton&) = delete;
	GPGamePlayMgrSingleton& operator=(const GPGamePlayMgrSingleton&) = delete;

private:
	GPGamePlayMgrSingleton() = default;
	~GPGamePlayMgrSingleton() = default;

	inline static char global_heap_buf[512 * 1024];
	static GPGamePlayMgrSingleton instance; // 声明静态实例
	inline static GPGamePlayModule* currentModule;
};


