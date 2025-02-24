// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GPGameMode.generated.h"

/**
 * 
 */
UCLASS()
class OZ_LIB_API AGPGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGPGameMode();
	
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
};
