// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "GPPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class OZ_LIB_API AGPPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
public:
	AGPPlayerCameraManager(const FObjectInitializer& ObjectInitializer);
	
	virtual void UpdateCamera(float DeltaTime) override;

	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;

	virtual void UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Category = "GPPlayerCameraManager")
	FRotator DefaultRotation;
};
