// Fill out your copyright notice in the Description page of Project Settings.


#include "GPPawn.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GPGameInstance.h"
#include "GPWrapper.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

// Sets default values
AGPPawn::AGPPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f / 30.0f;
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	AutoPossessAI = EAutoPossessAI::Disabled;
	AIControllerClass = nullptr;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	IMC_Local_PLayer = Cast<UInputMappingContext>(StaticLoadAsset(UInputMappingContext::StaticClass(), FTopLevelAssetPath(TEXT("/Game/Input/IMC_Player.IMC_Player")), LOAD_None));
	IA_Move = Cast<UInputAction>(StaticLoadAsset(UInputAction::StaticClass(), FTopLevelAssetPath(TEXT("/Game/Input/IA_Move.IA_Move")), LOAD_None));
	// IMC_Local_PLayer = TObjectPtr<UInputMappingContext>(FSoftObjectPath("/Game/Input/IMC_Player.IMC_Player"));
	// IA_Move = TObjectPtr<UInputAction>(FSoftObjectPath("/Game/Input/IA_Move.IA_Move"));
	Render_Generation = -1;
}

// Called when the game starts or when spawned
void AGPPawn::BeginPlay()
{
	UE_LOG(LogGamePlay, Log, TEXT("AGPPawn::BeginPlay"))
	Super::BeginPlay();
	GPWrapper::LoadDLL();
	Render_Generation = -1;
	
	// SetActorLocation(FVector(0.0f, 0.0f, 0.0f));
	// SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));

	SetupLocalInput();
	GPWrapper::Start();
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

	const InputData input_data = GetLocalInputData();
	const FGPRenderData render_data = GPWrapper::Update(input_data);
	
	ApplyRenderData(render_data);
}

void AGPPawn::SetupLocalInput()
{
	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(IMC_Local_PLayer.Get(), 0);
		}
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(IA_Move.Get(), ETriggerEvent::Triggered, this, &AGPPawn::OnInputMoveAxis2D);
		EnhancedInputComponent->BindAction(IA_Move.Get(), ETriggerEvent::Completed, this, &AGPPawn::OnInputMoveAxis2D);
	}
}

void AGPPawn::OnInputMoveAxis2D(const FInputActionValue& Value)
{
	FVector2D input = Value.Get<FVector2D>();
	Input_Move_Axis2D.X = input.Y;
	Input_Move_Axis2D.Y = input.X;
}

InputData AGPPawn::GetLocalInputData() const
{
	// 创建 InputData 实例
	InputData inputData;

	// 设置 player_0 的 state 为 Fighting，并指定 direction 和 magnitude
	inputData.player_0.state.tag = InputStateData_Fighting;
	inputData.player_0.state.data.fighting.x = Input_Move_Axis2D.X;
	inputData.player_0.state.data.fighting.y = Input_Move_Axis2D.Y;

	// 设置 player_0 的 action
	inputData.player_0.action.tag = InputActionData_Tag::None;
	inputData.player_0.action.data.choseCard = 1; // 根据需要设置 choseCard 的值

	return inputData;
}

void AGPPawn::ApplyRenderData(const FGPRenderData& render_data)
{
	if (render_data.generation != Render_Generation)
	{
		Render_Generation = render_data.generation;
		//     // 异步加载蓝图类
		//     FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		//     StreamableManager.RequestAsyncLoad(
		//         SoftObjectPath,
		//         FStreamableDelegate::CreateUObject(this, &ARandomMover::OnBlueprintLoaded)
		//     );
		// 生成角色
		// AActor* Actor = GetWorld()->SpawnActor<AActor>(BlueprintClass, SpawnLocation, SpawnRotation);
		// if (Actor)
		// {
		//     SpawnedActors.Add(Actor);
		//     UE_LOG(LogTemp, Log, TEXT("成功生成角色：%s"), *Actor->GetName());
		// }
	}
}

void AGPPawn::SpawnActor(int ModelID, const FVector& Location, const FRotator& Rotation)
{
	bool IsLocalCharacter = true;
	
	FString BlueprintPath = TEXT("/Game/Mixamo/Michelle/BP_Michelle");
	// 将字符串路径转换为 FSoftObjectPath
	FSoftObjectPath SoftObjectPath(BlueprintPath);
	if (!SoftObjectPath.IsValid())
	{
	    UE_LOG(LogTemp, Error, TEXT("蓝图路径无效：%s"), *BlueprintPath);
	    return;
	}

	// 获取加载的蓝图类
	UClass* BlueprintClass = LoadClass<AActor>(nullptr, *BlueprintPath);
	if (BlueprintClass == nullptr)
	{
	    UE_LOG(LogTemp, Error, TEXT("无法加载蓝图类：%s"), *BlueprintPath);
	    return;
	}
	
	UE_LOG(LogTemp, Log, TEXT("成功加载蓝图类：%s"), *BlueprintClass->GetName());
	
	UE_LOG(LogTemp, Log, TEXT("开始异步加载蓝图：%s"), *BlueprintPath);
	
	// 异步加载蓝图类
	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
	StreamableManager.RequestAsyncLoad(
	    SoftObjectPath,
	    FStreamableDelegate::CreateLambda([this, IsLocalCharacter, BlueprintClass, Location, Rotation]()
		{
			UE_LOG(LogTemp, Log, TEXT("蓝图加载完"));
	    	// 生成角色
			if (AActor* Actor = GetWorld()->SpawnActor<AActor>(BlueprintClass, Location, Rotation))
			{
				 SpawnedActors.Add(Actor);
				 UE_LOG(LogTemp, Log, TEXT("成功生成角色：%s"), *Actor->GetName());

				if(IsLocalCharacter)
				{
					LocalActor = Actor;
				}
			}
		})
	);
}