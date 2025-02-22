// Fill out your copyright notice in the Description page of Project Settings.


#include "GPWrapper.h"
#include "RenderDataWrapper.h"
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
	// auto logMessage = UTF8_TO_TCHAR(reinterpret_cast<const UTF8CHAR*>(msg));
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
	gp_update = static_cast<uint8*(*)(InputData)>(FPlatformProcess::GetDllExport(gp_dll_handle, TEXT("update")));
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

const GPRenderData* GPWrapper::Update(const InputData& inputData)
{
    const uint8* buffer = gp_update(inputData);
	const GPRenderData* renderData = GetGPRenderData(buffer);
	return renderData;
	// // 检查反序列化是否成功
	// if (!renderData)
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("Failed to deserialize GPRenderData!"));
	// }
	// else
	// {
	// 	int32 generation = renderData->generation();
	// 	UE_LOG(LogTemp, Log, TEXT("Generation: %d"), generation);
	// 	
	// 	const gameplay::GPRenderCharacterData* character0 = renderData->character0();
	// 	if (character0)
	// 	{
	// 		UE_LOG(LogTemp, Log, TEXT("Character0 ID: %d"), character0->id());
	// 		UE_LOG(LogTemp, Log, TEXT("Character0 Position: (%f, %f)"), character0->transform()->pos().x(), character0->transform()->pos().y());
	// 	}
	//
	// 	const flatbuffers::Vector<flatbuffers::Offset<gameplay::GPRenderCharacterData>>* monsters = renderData->monsters();
	// 	if (monsters)
	// 	{
	// 		for (const auto& monster : *monsters)
	// 		{
	// 			UE_LOG(LogTemp, Log, TEXT("Monster ID: %d"), monster->id());
	// 			UE_LOG(LogTemp, Log, TEXT("Monster Position: (%f, %f)"), monster->transform()->pos().x(), monster->transform()->pos().y());
	// 		}
	// 	}
	// }
	//
	// FGPRenderData RenderData;
	// return RenderData;
}


