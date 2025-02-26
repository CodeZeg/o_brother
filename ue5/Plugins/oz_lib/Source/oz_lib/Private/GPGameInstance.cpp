// Fill out your copyright notice in the Description page of Project Settings.


#include "GPGameInstance.h"
#include "Interfaces/IPluginManager.h"
// #include "WAMRWrapper.h"
#include "wasm_export.h"
#include <stdlib.h>
#include <string.h>

DEFINE_LOG_CATEGORY(LogGamePlay);

uint8* GetGamePlayWasmFileContent(uint32 *ret_size)
{
    // 获取插件基目录（如：/Project/Plugins/YourPlugin/）
    FString PluginDir = IPluginManager::Get().FindPlugin("oz_lib")->GetBaseDir();

    // 拼接文件路径
    FString FilePath = FPaths::Combine(*PluginDir, TEXT("ThirdParty/Win64/testapp.aot"));

	UE_LOG(LogGamePlay, Log, TEXT("Load wasm file: %s"), *FilePath);
	
    // 验证文件存在性
    if (!FPaths::FileExists(FilePath))
    {
        UE_LOG(LogGamePlay, Error, TEXT("文件未找到: %s"), *FilePath);
        uint8* empty = static_cast<uint8*>(malloc(1));  // 分配空间存放一个字符
    	if (empty != NULL) {
    	    empty[0] = '\0';  // 设置为一个空字符串
    	}
        return empty;
    }

	TArray<uint8> FileRawContent;
    if (!FFileHelper::LoadFileToArray(FileRawContent, *FilePath))
    {
    	UE_LOG(LogTemp, Error, TEXT("读取失败"));
    	return NULL;
    }

	const uint8* DataPtr = FileRawContent.GetData();

    *ret_size = static_cast<uint32_t>(FileRawContent.Num());

    FString HexHeader = BytesToHex(DataPtr, FMath::Min(32, FileRawContent.Num()));
	UE_LOG(LogGamePlay, Log, TEXT("WASM Header: %s"), *HexHeader);
	
	uint8_t* newDataPtr = new uint8_t[*ret_size];
	memcpy(newDataPtr, DataPtr, static_cast<size_t>(*ret_size) * sizeof(uint8_t));
    return newDataPtr;
}

void UGPGameInstance::Init()
{
	Super::Init();
	// WAMRWrapper::LoadDLL();

	// RuntimeInitArgs init_args;
	static char global_heap_buf[512 * 1024];
	uint32 buf_size;
	char error_buf[128];
	wasm_module_t module = NULL;

	wasm_module_inst_t module_inst = NULL;
	wasm_exec_env_t exec_env = NULL;
	uint32 stack_size = 8092, heap_size = 8092;
	wasm_function_inst_t func = NULL;
	
    // memset(&init_args, 0, sizeof(RuntimeInitArgs));
    // init_args.mem_alloc_type = Alloc_With_Pool;
    // static char global_heap_buf[512 * 1024];
    // init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
    // init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);

	// wasm_runtime_full_init(&init_args);
	RuntimeInitArgs init_args;
	memset(&init_args, 0, sizeof(RuntimeInitArgs));
	init_args.mem_alloc_type = Alloc_With_Pool;
	init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
	init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);

	if (!wasm_runtime_full_init(&init_args)) {
		UE_LOG(LogGamePlay, Log, TEXT("wasm_runtime_full_init failed."));
		return;
	}
	UE_LOG(LogGamePlay, Log, TEXT("full init wasm runtime ok..."));
	
	wasm_runtime_set_log_level(WASM_LOG_LEVEL_VERBOSE);
	UE_LOG(LogGamePlay, Log, TEXT("set wasm runtime log level ok..."));

	uint8* buffer = GetGamePlayWasmFileContent(&buf_size);

    if (!buffer) {
       UE_LOG(LogGamePlay, Log, TEXT("Open wasm app file failed."));
       return;
    }

	UE_LOG(LogGamePlay, Log, TEXT("read wasm file content ok, header:%x, %x, %x, %x..."), buffer[0], buffer[1], buffer[2], buffer[3]);
    UE_LOG(LogGamePlay, Log, TEXT("read wasm file content ok, len:%d..."), buf_size);

	try {
		module = wasm_runtime_load(buffer, buf_size, error_buf, sizeof(error_buf));

		// module = wasm_runtime_load(buffer, buf_size, error_buf, sizeof(error_buf));
        if (!module) {
        	FString FStringFromChar(error_buf);
            UE_LOG(LogGamePlay, Log, TEXT("Load wasm module failed. error:%s"), *FStringFromChar);
            return;
        }
        
        module_inst = wasm_runtime_instantiate(module, 65535, 0, error_buf, sizeof(error_buf));
        if (!module_inst) {
        	UE_LOG(LogGamePlay, Log, TEXT("Instantiate wasm module failed. error: %hs"), error_buf);
        	return;
        }
        	
        exec_env = wasm_runtime_create_exec_env(module_inst, stack_size);
        if (!exec_env) {
        	UE_LOG(LogGamePlay, Log, TEXT("Create wasm execution environment failed."));
        	return;
        }

		UE_LOG(LogGamePlay, Log, TEXT("wasm_runtime_create_exec_env ok"));
        
        func = wasm_runtime_lookup_function(module_inst, "generate_float");
        if (!func) {
        	UE_LOG(LogGamePlay, Log, TEXT("The generate_float wasm function is not found."));
        	return;
        }

		UE_LOG(LogGamePlay, Log, TEXT("wasm_runtime_lookup_function ok"));
		
		wasm_val_t results[1];
		results[0].kind = WASM_F32;
		results[0].of.f32 = 0;
	
		wasm_val_t arguments[3];
		arguments[0].kind =	WASM_I32;
		arguments[1].kind =	WASM_F64;
		arguments[2].kind =	WASM_F32;
		arguments[0].of.i32 = 10;
		arguments[1].of.f64 = 0.000101;
		arguments[2].of.f32 = 300.002;

		// pass 4 elements for function arguments
		if (!wasm_runtime_call_wasm_a(exec_env, func, 1, results, 3, arguments)) {
			UE_LOG(LogGamePlay, Log, TEXT("call wasm function generate_float failed. %hs"),
				   wasm_runtime_get_exception(module_inst));
			return;
		}

		float ret_val;
		ret_val = results[0].of.f32;
		UE_LOG(LogGamePlay, Log, TEXT("Native finished calling wasm function generate_float(), returned a "
			   "float value: %ff"), ret_val);	
	} catch (const std::exception& e) {
		UE_LOG(LogGamePlay, Log, TEXT("Custom error: %s"), UTF8_TO_TCHAR(e.what()));
	}


	UE_LOG(LogGamePlay, Log, TEXT("GPGameInstance::Init"));
}

void UGPGameInstance::Shutdown()
{
	Super::Shutdown();
	UE_LOG(LogGamePlay, Log, TEXT("GPGameInstance::Shutdown"));
}
void UGPGameInstance::StartGameInstance()
{
	Super::StartGameInstance();
	UE_LOG(LogGamePlay, Log, TEXT("GPGameInstance::StartGameInstance"));
}

