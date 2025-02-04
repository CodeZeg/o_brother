// Fill out your copyright notice in the Description page of Project Settings.


#include "GPWrapper.h"

#include "Interfaces/IPluginManager.h"

DEFINE_LOG_CATEGORY(LogGP);

static void log_wrapper(const uint8* msg, uint32 len)
{
	// 将消息转换为字符串并使用 Unreal 的日志系统
	auto logMessage = UTF8_TO_TCHAR(reinterpret_cast<const UTF8CHAR*>(msg));
	UE_LOG(LogGP, Display, TEXT("%s"), logMessage);
}

static void error_wrapper(const uint8* msg, uint32 len)
{
	// 将消息转换为字符串并使用 Unreal 的日志系统
	auto logMessage = UTF8_TO_TCHAR(reinterpret_cast<const UTF8CHAR*>(msg));
	UE_LOG(LogGP, Error, TEXT("%s"), logMessage);
}

static void Init_Engine(void* gp_dll_handle)
{
	struct EngineFunctions {
		void (*log)(const uint8_t* msg, uint32_t len);
		void (*error)(const uint8_t* msg, uint32_t len);
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
	gp_q_add = static_cast<int(*)(int, int)>(FPlatformProcess::GetDllExport(gp_dll_handle, TEXT("q_add")));
	gp_start = static_cast<void(*)()>(FPlatformProcess::GetDllExport(gp_dll_handle, TEXT("start")));
	gp_update = static_cast<void(*)(InputData)>(FPlatformProcess::GetDllExport(gp_dll_handle, TEXT("update")));
}

void GPWrapper::UnLoadDLL()
{ 
	if(gp_dll_handle == nullptr)
		return;

	UE_LOG(LogGP, Display, TEXT("UnLoad dll"));
	// Free the dll handle
	FPlatformProcess::FreeDllHandle(gp_dll_handle);
	gp_dll_handle = nullptr;
	gp_q_add = nullptr;
	gp_start = nullptr;
	gp_update = nullptr;
}

void GPWrapper::Start()
{
	int temp = gp_q_add(1561,312);
	UE_LOG(LogGP, Display, TEXT("q_add: %d"), temp);
	gp_start();
	UE_LOG(LogGP, Display, TEXT("gp_start: %d"), temp);
}

void GPWrapper::Update(float DeltaTime)
{
    // 创建 InputData 实例
    InputData inputData;

    // 设置 player_0 的 state 为 Fighting，并指定 direction 和 magnitude
    inputData.player_0.state.tag = InputStateData_Fighting;
    inputData.player_0.state.data.fighting.direction = 0.5f; // 设置 direction 为 0.5
    inputData.player_0.state.data.fighting.magnitude = DeltaTime; // 可以根据需要设置 magnitude

    // 设置 player_0 的 action
    inputData.player_0.action.tag = InputActionData_ChoseCard;
    inputData.player_0.action.data.choseCard = 1; // 根据需要设置 choseCard 的值
	
	gp_update(inputData);
}


