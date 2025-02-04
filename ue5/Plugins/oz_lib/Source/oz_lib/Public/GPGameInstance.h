// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GPGameInstance.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGamePlay, Log, All);

/**
 * 
 */
UCLASS()
class OZ_LIB_API UGPGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Shutdown() override;
	virtual void StartGameInstance() override;
};
