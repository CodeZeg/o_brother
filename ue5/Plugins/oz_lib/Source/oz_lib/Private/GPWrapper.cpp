// Fill out your copyright notice in the Description page of Project Settings.


#include "GPWrapper.h"
#include "render_data_generated.h"

#include "Interfaces/IPluginManager.h"

#include "GPGamePlay.h"

DEFINE_LOG_CATEGORY(LogGP);

FString getWasmFilePath(FString fileName)
{
	// 获取插件基目录（如：/Project/Plugins/YourPlugin/）
	FString PluginDir = IPluginManager::Get().FindPlugin("oz_lib")->GetBaseDir();
	// 拼接文件路径
	FString FilePath = FPaths::Combine(*PluginDir, TEXT("ThirdParty/Win64/" + fileName));
	return FilePath;
}


void GPWrapper::LoadDLL()
{
	// 初始化gameplay单例
	GPGamePlayMgrSingleton::GetInstance().Init();
	GPGamePlayMgrSingleton::GetInstance().LoadModule(getWasmFilePath(TEXT("gameplay.wasm")), 1024*10, 1024*10);
}

void GPWrapper::UnLoadDLL()
{ 
	GPGamePlayMgrSingleton::GetInstance().Release();
}

void GPWrapper::Start()
{
	(*GPGamePlayMgrSingleton::GetInstance().GetLoadedModule()).Call_Start();
	UE_LOG(LogTemp, Log, TEXT("wrapper call wasm start ok"));
}

const GPRenderData* GPWrapper::Update(const flatbuffers::FlatBufferBuilder& input_builder)
{
	UE_LOG(LogTemp, Log, TEXT("wrapper prepare call wasm update"));
    const uint8* buffer = (*GPGamePlayMgrSingleton::GetInstance().GetLoadedModule()).Call_Update(input_builder);
	UE_LOG(LogTemp, Log, TEXT("wrapper call wasm update ok"));
	const GPRenderData* renderData = GetGPRenderData(buffer);
	return renderData;
}


