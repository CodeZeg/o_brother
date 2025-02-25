// Fill out your copyright notice in the Description page of Project Settings.


#include "GPWrapper.h"
#include "render_data_generated.h"

#include "Interfaces/IPluginManager.h"

DEFINE_LOG_CATEGORY(LogGP);

static void log_wrapper(const char* msg, uint32 len)
{
	// 将消息转换为字符串并使用 Unreal 的日志系统
	auto logMessage = FString(msg);
	UE_LOG(LogGP, Display, TEXT("%d, %s"), len, *logMessage);
}

static void error_wrapper(const char* msg, uint32 len)
{
	// 将消息转换为字符串并使用 Unreal 的日志系统
	auto logMessage = FString(msg);
	UE_LOG(LogGP, Error, TEXT("%d, %s"), len, *logMessage);
}

static void Init_Engine(void* gp_dll_handle)
{
	struct EngineFunctions {
		void (*log)(const char* msg, uint32_t len);
		void (*error)(const char* msg, uint32_t len);
	};
	typedef void (*InitEngineFunc)(EngineFunctions);
	InitEngineFunc init_logger = (InitEngineFunc)FPlatformProcess::GetDllExport(gp_dll_handle, TEXT("init_engine"));
	if (init_logger)
	{
		EngineFunctions funcs;
		funcs.log = &log_wrapper;
		funcs.error = &error_wrapper;
		init_logger(funcs);
	}

}

void GPWrapper::LoadDLL()
{
	if(gp_dll_handle)
		return;

	FString BaseDir = IPluginManager::Get().FindPlugin("oz_lib")->GetBaseDir();
	// Add on the relative location of the third party dll and load it
	FString LibraryPath;
#if PLATFORM_WINDOWS
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("ThirdParty/Win64/gameplay.dll"));
#endif // PLATFORM_WINDOWS

	UE_LOG(LogGP, Display, TEXT("Load dll: %s"), *LibraryPath);
	gp_dll_handle = FPlatformProcess::GetDllHandle(*LibraryPath);
	Init_Engine(gp_dll_handle);
	gp_start = static_cast<void(*)()>(FPlatformProcess::GetDllExport(gp_dll_handle, TEXT("start")));
	gp_update = static_cast<const uint8*(*)(const uint8*, uint32)>(FPlatformProcess::GetDllExport(gp_dll_handle, TEXT("update")));
}

void GPWrapper::UnLoadDLL()
{ 
	if(gp_dll_handle == nullptr)
		return;

	UE_LOG(LogGP, Display, TEXT("UnLoad dll"));
	// Free the dll handle
	FPlatformProcess::FreeDllHandle(gp_dll_handle);
	gp_dll_handle = nullptr;
	gp_start = nullptr;
	gp_update = nullptr;
}

void GPWrapper::Start()
{
	gp_start();
}

const GPRenderData* GPWrapper::Update(const flatbuffers::FlatBufferBuilder& input_builder)
{
	const uint8* input_buffer = input_builder.GetBufferPointer();
	const int input_buffer_size = input_builder.GetSize();
    const uint8* buffer = gp_update(input_buffer, input_buffer_size);
	const GPRenderData* renderData = GetGPRenderData(buffer);
	return renderData;
}


