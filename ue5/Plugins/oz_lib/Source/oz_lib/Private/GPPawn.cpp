// Fill out your copyright notice in the Description page of Project Settings.


#include "GPPawn.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GPGameInstance.h"
#include "GPWrapper.h"
#include "input_data_generated.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
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
	Actor_Render_Generation = -1;
}

// Called when the game starts or when spawned
void AGPPawn::BeginPlay()
{
	UE_LOG(LogGamePlay, Log, TEXT("AGPPawn::BeginPlay"))
	Super::BeginPlay();
	GPWrapper::LoadDLL();
	Actor_Render_Generation = -1;
	Effect_Render_Generation = -1;
	
	SetActorLocation(FVector(0.0f, 0.0f, 0.0f));
	SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));

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

	flatbuffers::FlatBufferBuilder builder;
	flatbuffers::Offset<GPInputData> input_data = GetLocalInputData(builder);
	builder.Finish(input_data);
	const GPRenderData* render_data = GPWrapper::Update(builder);
	
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


flatbuffers::Offset<GPInputData> AGPPawn::GetLocalInputData(flatbuffers::FlatBufferBuilder &builder) const
{
	auto move_data = CreateGPInputMoveData(builder, Input_Move_Axis2D.X, Input_Move_Axis2D.Y);
	auto action_data = CreateInputAction_ChoseCard(builder, 1); // 设置 choseCard 的值
	auto player_data = CreateGPInputPlayerData(builder,
		GPInputStateData_GPInputMoveData, move_data.Union(),
		GPInputActionData_InputAction_ChoseCard, action_data.Union()
		); 
	auto players = builder.CreateVector({player_data});
	auto input_data = CreateGPInputData(builder, players);
	return input_data;
}

void ApplyCharacterRenderData(const TObjectPtr<AActor>& Actor, const GPRenderCharacterData* RenderData)
{
	if (!Actor->IsValidLowLevel())
		return;
	
	Actor->SetActorLocationAndRotation(GetPosition(RenderData->transform()), GetRotation(RenderData->transform()));
	const USkeletalMeshComponent* SkeletalMeshComponent = Actor->GetComponentByClass<USkeletalMeshComponent>();
	if (!SkeletalMeshComponent)
	{
	    UE_LOG(LogTemp, Error, TEXT("未找到 SkeletalMeshComponent！"));
	    return;
	}
	UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
	if (!AnimInstance)
	{
	    UE_LOG(LogTemp, Error, TEXT("未找到 AnimInstance！"));
	    return;
	}

	const FName ParameterName = TEXT("Speed"); // 替换为你的混合树参数名称
	const float ParameterValue = RenderData->motion_state()->locomotion_speed();
	FProperty* Property = AnimInstance->GetClass()->FindPropertyByName(ParameterName);
	if (Property && Property->IsA<FDoubleProperty>())
	{
		if (const FDoubleProperty* FloatProperty = CastField<FDoubleProperty>(Property))
	    {
	        // 设置属性值
	        FloatProperty->SetPropertyValue_InContainer(AnimInstance, ParameterValue);
	        // UE_LOG(LogTemp, Log, TEXT("设置 %s 为 %f"), *ParameterName.ToString(), ParameterValue);
	    }
	}
	else
	{
	    UE_LOG(LogTemp, Warning, TEXT("找不到 Float 类型的属性 %s"), *ParameterName.ToString());
	}
}

void AGPPawn::PlayEffect(const GPRenderEffectData* EffectData)
{
	const int32 EffectID = EffectData->effect_id();
	const FVector Location = GetPosition(EffectData->transform());
	const FRotator Rotation = GetRotation(EffectData->transform());
	FString NiagaraSystemPath;
	FVector Scale = FVector::OneVector;
	if (EffectData->effect_res_id() == 10001)
	{
		NiagaraSystemPath = TEXT("/Game/Vefects/Knife_light/VFX/NE_attack02.NE_attack02");
		Scale *= 2.0f;
	}
	else //if (EffectData->effect_res_id() == 20001)
	{
		NiagaraSystemPath = TEXT("/Game/Vefects/Zap_VFX/VFX/Zap/Particles/NS_Zap_01_Red.NS_Zap_01_Red");
	}

	UE_LOG(LogGamePlay, Display, TEXT("PlayEffect: %s，ID: %d"), *NiagaraSystemPath, EffectID);
	
	// 加载NiagaraSystem
	UNiagaraSystem* NiagaraSystem = LoadObject<UNiagaraSystem>(nullptr, *NiagaraSystemPath);
	if (!NiagaraSystem)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load NiagaraSystem at path: %s"), *NiagaraSystemPath);
		return;
	}

	// 获取当前世界的上下文
	const UWorld* World = GetWorld();

	// 创建并播放Niagara特效
	UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, NiagaraSystem, Location, Rotation, Scale);
	if (!NiagaraComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn NiagaraSystem at location"));
		return;
	}

	SpawnedEffects.Add(EffectID, NiagaraComponent);
}

void AGPPawn::ApplyRenderData(const GPRenderData* render_data)
{
	const auto actors = render_data->actors();
	// 增删 actor
	if (render_data->actors_generation() != Actor_Render_Generation)
	{
		Actor_Render_Generation = render_data->actors_generation();
		TSet<int32> SrcIDs;
		SpawnedActors.GetKeys(SrcIDs);
		TSet<int32> DstIDs;
		for(auto it = actors->begin(); it!= actors->end(); ++it)
		{
			DstIDs.Add(it->actor_id());
		}
		
		// 删除, 之前有的, 但是新的中没有的
		for (const int32& SrcID : SrcIDs)
		{
			if (!DstIDs.Contains(SrcID))
			{
				if (const TObjectPtr<AActor> Actor = SpawnedActors.FindRef(SrcID))
				{
					if(Actor->IsValidLowLevelFast())
					{
						Actor->Destroy();
					}
					SpawnedActors.Remove(SrcID);
				}
			}
		}
		
		// 新增, 新的中有的, 但是之前没有的
		for(auto it = actors->begin(); it!= actors->end(); ++it)
		{
			if (!SrcIDs.Contains(it->actor_id()))
			{
				SpawnActor(*it);
			}
		}
	}

	// 修改, 更新每个actor的数据
	for(auto it = actors->begin(); it!= actors->end(); ++it)
	{
		auto UEActor = SpawnedActors.FindRef(it->actor_id());
		ApplyCharacterRenderData(UEActor, *it);
	}

	// 本地玩家控制角色
	if(!LocalActor->IsValidLowLevel())
	{
		LocalActor = SpawnedActors.FindRef(1001);
		if(LocalActor)
		{
			RootComponent = LocalActor->GetRootComponent();
		}
	}

	// 增删 effect
	if (render_data->effects_generation() != Effect_Render_Generation)
	{
		Effect_Render_Generation = render_data->effects_generation();
		TSet<int32> SrcIDs;
		SpawnedEffects.GetKeys(SrcIDs);
		TSet<int32> DstIDs;
		const auto effects = render_data->effects();
		for(auto it = effects->begin(); it!= effects->end(); ++it)
		{
			DstIDs.Add(it->effect_id());
		}
		
		// 删除, 之前有的, 但是新的中没有的
		for (const int32& SrcID : SrcIDs)
		{
			if (!DstIDs.Contains(SrcID))
			{
				if (const TObjectPtr<UNiagaraComponent> Effect = SpawnedEffects.FindRef(SrcID))
				{
					if(Effect->IsValidLowLevelFast())
					{
						Effect->DestroyComponent();
					}
					SpawnedEffects.Remove(SrcID);
				}
			}
		}
		
		// 新增, 新的中有的, 但是之前没有的
		for(auto it = effects->begin(); it!= effects->end(); ++it)
		{
			if (!SrcIDs.Contains(it->effect_id()))
			{
				PlayEffect(*it);
			}
		}
	}
}

void AGPPawn::SpawnActor(const GPRenderCharacterData* character)
{
	const auto actor_id = character->actor_id();
	const auto actor_res_id = character->actor_res_id();
	FString BlueprintPath;
	if (actor_res_id == 1001)
	{
		BlueprintPath = TEXT("/Game/Blueprints/Characters/BP_Michelle.BP_Michelle_C");
	}
	else
	{
		BlueprintPath = TEXT("/Game/Blueprints/Characters/BP_Ortiz.BP_Ortiz_C");
	}

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
	
	// UE_LOG(LogTemp, Log, TEXT("成功加载蓝图类：%s"), *BlueprintClass->GetName());
	// UE_LOG(LogTemp, Log, TEXT("开始异步加载蓝图：%s"), *BlueprintPath);
	FVector SpawnLocation = GetPosition(character->transform());
	FRotator SpawnRotation = GetRotation(character->transform());
	
	// 异步加载蓝图类
	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
	StreamableManager.RequestAsyncLoad(
	    SoftObjectPath,
	    FStreamableDelegate::CreateLambda([this, actor_id, BlueprintClass, SpawnLocation, SpawnRotation]()
		{
			// UE_LOG(LogTemp, Log, TEXT("蓝图加载完"));
	    	// 生成角色
			if (AActor* Actor = GetWorld()->SpawnActor<AActor>(BlueprintClass, SpawnLocation, SpawnRotation))
			{
				 SpawnedActors.Add(actor_id, Actor);
				 // UE_LOG(LogTemp, Log, TEXT("成功生成角色：%s"), *Actor->GetName());
			}
		})
	);
}