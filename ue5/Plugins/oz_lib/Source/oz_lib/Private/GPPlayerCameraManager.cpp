// Fill out your copyright notice in the Description page of Project Settings.


#include "GPPlayerCameraManager.h"


AGPPlayerCameraManager::AGPPlayerCameraManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultFOV = 55.0f;
	DefaultAspectRatio = 16.0f / 9.0f;
	FreeCamDistance = 1400.0f; // 自由相机距离
	DefaultRotation = FRotator(-50.0f, 0.0f, 0.0f); // 默认旋转角度
}

void AGPPlayerCameraManager::UpdateCamera(float DeltaTime)
{
	Super::UpdateCamera(DeltaTime);
}

void AGPPlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	Super::UpdateViewTarget(OutVT, DeltaTime);
}

void AGPPlayerCameraManager::UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime)
{
	FVector Loc = OutVT.Target->GetActorLocation();
	// FRotator Rotator = OutVT.Target->GetActorRotation();
	FRotator Rotator = DefaultRotation;
	// if (OutVT.Target == PCOwner)
	// {
	// 	Loc = PCOwner->GetFocalLocation();
	// }
	
	FVector Pos = Loc + ViewTargetOffset + FRotationMatrix(Rotator).TransformVector(FreeCamOffset) - Rotator.Vector() * FreeCamDistance;
	OutVT.POV.Location = Pos;
	OutVT.POV.Rotation = Rotator;
}
