#include "GPGamePlay.h"
#include "utils.h"
#include "Interfaces/IPluginManager.h"
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Logging/LogMacros.h"

DEFINE_LOG_CATEGORY(LogGP);

// 类外初始化单例（程序启动时执行）
GPGamePlayMgrSingleton GPGamePlayMgrSingleton::instance;

GPGamePlayModule::GPGamePlayModule(FString FilePathArg, uint32_t StackSizeArg = 8092, uint32_t HeadpSizeArg = 8092)
{
	this->FilePath = FilePathArg;
	this->StackSize = StackSizeArg;
	this->HeapSize = HeadpSizeArg;
	return;
}

GPGamePlayModule& GPGamePlayModule::Load()
{
	uint32 contentSize;
	uint8* contentBuffer;
	char error_buf[128];
	wasm_module_t module = NULL;
	contentBuffer = ReadFileContent(FilePath, &contentSize);
	if (!contentBuffer)
	{
		UE_LOG(LogTemp, Error, TEXT("read file %s failed"), *FilePath)
		return *this;
	}

	UE_LOG(LogTemp, Log, TEXT("read wasm file %s content ok"), *FilePath);
	
	module = wasm_runtime_load(contentBuffer, contentSize, error_buf, sizeof(error_buf));
	if (!module) {
		FString FStringFromChar(error_buf);
		UE_LOG(LogTemp, Error, TEXT("load wasm module error:%s"), *FStringFromChar)
		return *this;
	}
	
	UE_LOG(LogTemp, Log, TEXT("wasm_runtime_load ok"));
        
	ModuleInstance = wasm_runtime_instantiate(module, StackSize, HeapSize, error_buf, sizeof(error_buf));
	if (!ModuleInstance) {
		UE_LOG(LogTemp, Error, TEXT("Instantiate wasm module failed. error: %hs"), error_buf);
		return *this;
	}

	UE_LOG(LogTemp, Log, TEXT("wasm_runtime_instantiate ok"));

	ExecEnv = wasm_runtime_create_exec_env(ModuleInstance, StackSize);
	if (!ExecEnv) {
		UE_LOG(LogTemp, Error, TEXT("create wasm runtime exec env failed"))
		return *this;
	}
	
	UE_LOG(LogTemp, Log, TEXT("wasm_runtime_create_exec_env ok"));

	WasmFunStart = wasm_runtime_lookup_function(ModuleInstance, "start");
	if (!WasmFunStart) {
		UE_LOG(LogTemp, Error, TEXT("lookup function start failed"))
		return *this;
	}
	
	UE_LOG(LogTemp, Log, TEXT("wasm_runtime_lookup_function start ok"));

	WasmFunUpdate = wasm_runtime_lookup_function(ModuleInstance, "update");
	if (!WasmFunUpdate) {
		UE_LOG(LogTemp, Error, TEXT("lookup function update failed"))
		return *this;
	}

	UE_LOG(LogTemp, Log, TEXT("wasm_runtime_lookup_function update ok"));
	
	return *this;
}

void GPGamePlayModule::Call_Start()
{
	uint32 argv2[1];
	
	if (!wasm_runtime_call_wasm(ExecEnv, WasmFunStart, 0, argv2)) {
		UE_LOG(LogTemp, Warning, TEXT("call wasm function start failed. %hs"),
			   wasm_runtime_get_exception(ModuleInstance));
		return;
	}
}

uint8* GPGamePlayModule::Call_Update(const flatbuffers::FlatBufferBuilder& InputBuilder)
{
	// 序列化flatbuffer数据
	const uint8* input_buffer = InputBuilder.GetBufferPointer();
	const int input_buffer_size = InputBuilder.GetSize();

	// 从wasm虚拟机分配堆内存，保存输入数据buffer入参
	void *input_wasm_ptr = NULL;
	uint32_t input_wasm_offset = wasm_runtime_module_malloc(ModuleInstance, input_buffer_size, (void **)&input_wasm_ptr);
	if (input_wasm_offset == 0) {
		UE_LOG(LogTemp, Warning, TEXT("malloc vm memory for input data failed, req size:%d"), input_buffer_size);
		return NULL;
	}
	memcpy(input_wasm_ptr, input_buffer, input_buffer_size);

	// 从wasm虚拟机分配堆内存，保存返回数据buffer长度字段
	void *output_len_wasm_ptr = NULL;
	uint32_t output_len_wasm_offset = wasm_runtime_module_malloc(ModuleInstance, sizeof(uint32_t), (void **)&output_len_wasm_ptr);
	if (output_len_wasm_offset == 0) {
		UE_LOG(LogTemp, Warning, TEXT("malloc vm memory for output len failed, req size:%d"), sizeof(uint32_t));
		return NULL;
	}

	// 从wasm虚拟机分配堆内存，保存返回数据buffer
	void *ret_buffer = NULL;
	uint64_t wasm_ret_buffer_offset = wasm_runtime_module_malloc(ModuleInstance, 1024, (void **)&ret_buffer);
	if (wasm_ret_buffer_offset == 0) {
		UE_LOG(LogTemp, Warning, TEXT("malloc vm memory for ret buffer failed, req size:%d"), 1024);
		return NULL;
	}

	// 调用 wasm update方法
	UE_LOG(LogTemp, Log, TEXT("prepare all data, calling update."))
	uint32_t argv[4] = {input_wasm_offset, static_cast<uint32>(input_buffer_size), static_cast<uint32>(wasm_ret_buffer_offset), output_len_wasm_offset};
	if (!wasm_runtime_call_wasm(ExecEnv, WasmFunUpdate, 3, argv)) {
		UE_LOG(LogTemp, Warning, TEXT("Process failed: %hs"), wasm_runtime_get_exception(ModuleInstance));
		return NULL;
	}
	
	// 获取返回数据长度
	uint32_t output_len = *(uint32_t*)output_len_wasm_ptr;

	UE_LOG(LogTemp, Log, TEXT("call update ok, prepare handle response data, len:%d"), output_len);

	// 复制输出数据到宿主
	uint8_t *output_data = static_cast<uint8_t*>(malloc(output_len));
	memcpy(output_data, ret_buffer, output_len);

	{ // 释放所有虚拟机分配的内存数据
		wasm_runtime_module_free(ModuleInstance, input_wasm_offset);
		wasm_runtime_module_free(ModuleInstance, output_len_wasm_offset);
		wasm_runtime_module_free(ModuleInstance, wasm_ret_buffer_offset);
	}
	
	return output_data;
}

static void log_wrapper(wasm_exec_env_t exec_env, const char* msg, uint32 len)
{
	// 将消息转换为字符串并使用 Unreal 的日志系统
	auto logMessage = FString(msg);
	UE_LOG(LogGP, Display, TEXT("%d, %s"), len, *logMessage);
}

static void error_wrapper(wasm_exec_env_t exec_env, const char* msg, uint32 len)
{
	// 将消息转换为字符串并使用 Unreal 的日志系统
	auto logMessage = FString(msg);
	UE_LOG(LogGP, Error, TEXT("%d, %s"), len, *logMessage);
}


static NativeSymbol native_symbols[] = {
    { "log_wrapper", log_wrapper, "($i)", NULL },
    { "error_wrapper", error_wrapper, "($i)", NULL },
};

void GPGamePlayMgrSingleton::Init()
{	
 	RuntimeInitArgs init_args;
 	memset(&init_args, 0, sizeof(RuntimeInitArgs));
 	init_args.mem_alloc_type = Alloc_With_Pool;
 	init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
 	init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);

 	init_args.n_native_symbols = sizeof(native_symbols) / sizeof(NativeSymbol);
    init_args.native_module_name = "env";
    init_args.native_symbols = native_symbols;
 	if (!wasm_runtime_full_init(&init_args)) {
 		UE_LOG(LogTemp, Error, TEXT("init wasm runtime failed"))
 		return;
 	}
 	wasm_runtime_set_log_level(WASM_LOG_LEVEL_VERBOSE);
}

void GPGamePlayMgrSingleton::Release()
{
	
}

void GPGamePlayMgrSingleton::LoadModule(FString filePath, uint32_t stack_size, uint32_t heap_size)
{
	currentModule = &(GPGamePlayModule(filePath, stack_size, heap_size).Load());
}

GPGamePlayModule* GPGamePlayMgrSingleton::GetLoadedModule()
{
	// todo 可能会并发替换
	return currentModule;
}

