// Fill out your copyright notice in the Description page of Project Settings.


#include "GPGameInstance.h"
// #include "WAMRWrapper.h"
#include "wasm_export.h"

DEFINE_LOG_CATEGORY(LogGamePlay);

void UGPGameInstance::Init()
{
	Super::Init();
	// WAMRWrapper::LoadDLL();

	RuntimeInitArgs init_args;
    memset(&init_args, 0, sizeof(RuntimeInitArgs));
    init_args.mem_alloc_type = Alloc_With_Pool;
    static char global_heap_buf[512 * 1024];
    init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
    init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);

	wasm_runtime_full_init(&init_args);
	UE_LOG(LogGamePlay, Log, TEXT("full init wasm runtime ok..."));
	wasm_runtime_set_log_level(WASM_LOG_LEVEL_VERBOSE);
	UE_LOG(LogGamePlay, Log, TEXT("set wasm runtime log level ok1..."));
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