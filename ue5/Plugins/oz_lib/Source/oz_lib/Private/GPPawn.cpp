// Fill out your copyright notice in the Description page of Project Settings.


#include "GPPawn.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GPGameInstance.h"
#include "GPWrapper.h"


// Sets default values
AGPPawn::AGPPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	AutoPossessAI = EAutoPossessAI::Disabled;
	AIControllerClass = nullptr;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	IMC_Local_PLayer = TSoftObjectPtr<UInputMappingContext>(FSoftObjectPath("/Game/Input/IMC_Player.IMC_Player"));
	IA_Move = TSoftObjectPtr<UInputAction>(FSoftObjectPath("/Game/Input/IA_Move.IA_Move"));
}

// Called when the game starts or when spawned
void AGPPawn::BeginPlay()
{
	Super::BeginPlay();
	GPWrapper::LoadDLL();

	UE_LOG(LogGamePlay, Log, TEXT("AGPPawn::BeginPlay"))
	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		UE_LOG(LogGamePlay, Log, TEXT("LocalPlayerController: %s"), *PlayerController->GetName())
		// 获取本地玩家增强输入子系统
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// 添加映射上下文
			Subsystem->AddMappingContext(IMC_Local_PLayer.Get(), 0);
			UE_LOG(LogGamePlay, Log, TEXT("AddMappingContext: %s"), *IMC_Local_PLayer.ToString())
		}
	}
}

void AGPPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GPWrapper::UnLoadDLL();
}

// Called every frame
void AGPPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Location = GetActorLocation();
	Location.X += Input_Move_Axis2D.X * DeltaTime * 600.0f;
	Location.Y += Input_Move_Axis2D.Y * DeltaTime * 600.0f;
	SetActorLocation(Location);
}

// Called to bind functionality to input
void AGPPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 转换为增强输入组件
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 绑定动作
		EnhancedInputComponent->BindAction(IA_Move.Get(), ETriggerEvent::Triggered, this, &AGPPawn::OnInputMoveAxis2D);
		UE_LOG(LogGamePlay, Log, TEXT("BindAction: %s"), *IA_Move.ToString())
	}
}

void AGPPawn::OnInputMoveAxis2D(const FInputActionValue& Value)
{
	Input_Move_Axis2D = Value.Get<FVector2D>();
}

