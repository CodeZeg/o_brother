// Fill out your copyright notice in the Description page of Project Settings.


#include "GPGameInstance.h"

DEFINE_LOG_CATEGORY(LogGamePlay);

void UGPGameInstance::Init()
{
	UE_LOG(LogGamePlay, Log, TEXT("GPGameInstance::Init"));
}
void UGPGameInstance::Shutdown()
{
	UE_LOG(LogGamePlay, Log, TEXT("GPGameInstance::Shutdown"));
}
void UGPGameInstance::StartGameInstance()
{
	UE_LOG(LogGamePlay, Log, TEXT("GPGameInstance::StartGameInstance"));
}