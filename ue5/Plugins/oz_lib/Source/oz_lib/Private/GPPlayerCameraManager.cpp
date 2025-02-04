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

	// APawn* TargetPawn = PCOwner ? PCOwner->GetPawn() : nullptr;
	// if (!TargetPawn)
	// 	return;
	//
	// // 获取Pawn当前位置
	// FVector PawnLocation = TargetPawn->GetActorLocation();
 //        
	// // 限制Pawn在游戏区域内
	// PawnLocation.X = FMath::Clamp(PawnLocation.X, -GameAreaSize, GameAreaSize);
	// PawnLocation.Y = FMath::Clamp(PawnLocation.Y, -GameAreaSize, GameAreaSize);
	//
	// // 计算理想的摄像机位置
	// FVector TargetCameraLocation = FVector(
	// 	PawnLocation.X,
	// 	PawnLocation.Y,
	// 	CameraHeight
	// );
	//
	// // 平滑插值到目标位置
	// FVector CurrentLocation = GetCameraLocation();
	// FVector NewLocation = FMath::VInterpTo(
	// 	CurrentLocation,
	// 	TargetCameraLocation,
	// 	DeltaTime,
	// 	FollowSpeed
	// );
	//
	// // 设置摄像机位置和旋转
	// SetActorLocation(NewLocation);
	// SetActorRotation(FRotator(-90.0f, 0.0f, 0.0f)); // 正下方看
	//
	// // 确保视野范围合适
	// UpdateCameraFOV();
	//
	// // 在更新完摄像机后绘制调试信息
	// DrawDebugVisuals();
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
