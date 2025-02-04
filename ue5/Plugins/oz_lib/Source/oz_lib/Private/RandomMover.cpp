// #include "RandomMover.h"
//
// #include "EnhancedInputComponent.h"
// #include "EnhancedInputSubsystems.h"
// #include "Kismet/KismetMathLibrary.h"
// #include "TimerManager.h"
// #include "Engine/Blueprint.h"
// #include "UObject/ConstructorHelpers.h"
// #include "Engine/AssetManager.h"
// #include "Engine/StreamableManager.h"
// #include "Animation/AnimInstance.h"
// #include "GPWrapper.h"
//
// // Sets default values
// ARandomMover::ARandomMover()
// {
//     // Set this actor to call Tick() every frame.
//     PrimaryActorTick.bCanEverTick = true;
//
//     // 初始化输入映射上下文
//     IMC_Local_PLayer = nullptr;
//
//     /// 初始化输入映射
//     IA_Move = nullptr;
//
//     // 初始化移动轴
//     Move_Axis2D = FVector2D(0.0f, 0.0f);
//
//     // 默认生成 5 个角色
//     NumberOfActors = 5;
//
//     // 默认移动速度
//     MovementSpeed = 100.0f;
//
//     // 默认蓝图路径
//     BlueprintPath = TEXT("/Game/Mixamo/Michelle/BP_Michelle");
// }
//
// // Called when the game starts or when spawned
// void ARandomMover::BeginPlay()
// {
//     Super::BeginPlay();
//     GPWrapper::LoadDLL();
//     if(IMC_Local_PLayer)
//     {
//         if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
//         {
//             // 获取本地玩家增强输入子系统
//             if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
//             {
//                 // 添加映射上下文
//                 Subsystem->AddMappingContext(IMC_Local_PLayer, 0);
//             }
//         }
//     }
//     
//     // 初始生成角色
//     SpawnActors();
// }
//
// void ARandomMover::EndPlay(const EEndPlayReason::Type EndPlayReason)
// {
//     Super::EndPlay(EndPlayReason);
//     GPWrapper::UnLoadDLL();
// }
//
// // Called every frame
// void ARandomMover::Tick(float DeltaTime)
// {
//     Super::Tick(DeltaTime);
//     GPWrapper::Update(DeltaTime);
//
//     if (IA_Move)
//     {
//         // UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem =  GetWorld()->GetFirstLocalPlayerFromController()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
//         // UEnhancedPlayerInput* PlayerInput = EnhancedInputSubsystem->GetPlayerInput();
//         // PlayerInput->InjectInputForAction(IA_Move, Move_Axis2D);
//         UE_LOG(LogGP, Log, TEXT("移动输入：x:%f, y:%f"), Move_Axis2D.X, Move_Axis2D.Y);
//     }
//
//     // 每帧驱动角色做随机运动
//     ChangeDirections();
//
//     // UpdateDataStruct data;
//     // data.interval = DeltaTime;
//     // update(data);
// }
//
// // Function to change movement directions
// void ARandomMover::ChangeDirections()
// {
//     for (AActor* Actor : SpawnedActors)
//     {
//         if (Actor)
//         {
//             // // 生成一个随机方向向量
//             // FVector NewDirection = UKismetMathLibrary::RandomUnitVector() * MovementSpeed;
//             // NewDirection.Z = 0;
//             // // 设置角色的移动
//             // Actor->SetActorLocation(Actor->GetActorLocation() + NewDirection * GetWorld()->GetDeltaSeconds());
//             // 获取 SkeletalMeshComponent
//             USkeletalMeshComponent* SkeletalMeshComponent = Actor->GetComponentByClass<USkeletalMeshComponent>();
//             if (!SkeletalMeshComponent)
//             {
//                 UE_LOG(LogTemp, Error, TEXT("未找到 SkeletalMeshComponent！"));
//                 return;
//             }
//             UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
//             if (!AnimInstance)
//             {
//                 UE_LOG(LogTemp, Error, TEXT("未找到 AnimInstance！"));
//                 return;
//             }
//             
//             FName ParameterName = TEXT("Speed"); // 替换为你的混合树参数名称
//             float ParameterValue = FMath::FRandRange(0.0, 600.0);
//             FProperty* Property = AnimInstance->GetClass()->FindPropertyByName(ParameterName);
//             if (Property && Property->IsA<FDoubleProperty>())
//             {
//                 FDoubleProperty* FloatProperty = CastField<FDoubleProperty>(Property);
//                 if (FloatProperty)
//                 {
//                     // 设置属性值
//                     FloatProperty->SetPropertyValue_InContainer(AnimInstance, ParameterValue);
//                     UE_LOG(LogTemp, Log, TEXT("设置 %s 为 %f"), *ParameterName.ToString(), ParameterValue);
//                 }
//             }
//             else
//             {
//                 UE_LOG(LogTemp, Warning, TEXT("找不到 Float 类型的属性 %s"), *ParameterName.ToString());
//             }
//         }
//     }
// }
//
// void ARandomMover::SpawnActors()
// {
//     if (BlueprintPath.IsEmpty())
//     {
//         UE_LOG(LogTemp, Error, TEXT("蓝图路径为空！"));
//         return;
//     }
//
//     // 将字符串路径转换为 FSoftObjectPath
//     FSoftObjectPath SoftObjectPath(BlueprintPath);
//     if (!SoftObjectPath.IsValid())
//     {
//         UE_LOG(LogTemp, Error, TEXT("蓝图路径无效：%s"), *BlueprintPath);
//         return;
//     }
//
//     UE_LOG(LogTemp, Log, TEXT("开始异步加载蓝图：%s"), *BlueprintPath);
//
//     // 异步加载蓝图类
//     FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
//     StreamableManager.RequestAsyncLoad(
//         SoftObjectPath,
//         FStreamableDelegate::CreateUObject(this, &ARandomMover::OnBlueprintLoaded)
//     );
//
//     // InitDataStruct data;
//     // data.value = 456;
//     // xxx::start();
//     // q_add(12, 55);
//     GPWrapper::Start();
// }
//
// void ARandomMover::OnBlueprintLoaded()
// {
//     UE_LOG(LogTemp, Log, TEXT("蓝图加载完成：%s"), *BlueprintPath);
//
//     // 获取加载的蓝图类
//     UClass* BlueprintClass = LoadClass<AActor>(nullptr, *BlueprintPath);
//     if (BlueprintClass == nullptr)
//     {
//         UE_LOG(LogTemp, Error, TEXT("无法加载蓝图类：%s"), *BlueprintPath);
//         return;
//     }
//
//     UE_LOG(LogTemp, Log, TEXT("成功加载蓝图类：%s"), *BlueprintClass->GetName());
//
//     // 生成角色
//     for (int32 i = 0; i < NumberOfActors; ++i)
//     {
//         // 生成随机位置
//         FVector SpawnLocation = UKismetMathLibrary::RandomPointInBoundingBox(GetActorLocation(), FVector(500.f, 500.f, 0.f));
//         FRotator SpawnRotation = FRotator::ZeroRotator;
//
//         // 生成角色
//         AActor* Actor = GetWorld()->SpawnActor<AActor>(BlueprintClass, SpawnLocation, SpawnRotation);
//         if (Actor)
//         {
//             SpawnedActors.Add(Actor);
//             UE_LOG(LogTemp, Log, TEXT("成功生成角色：%s"), *Actor->GetName());
//         }
//     }
// }
//
// // Function to destroy all spawned actors
// void ARandomMover::DestroyActors()
// {
//     for (AActor* Actor : SpawnedActors)
//     {
//         if (Actor)
//         {
//             Actor->Destroy();
//         }
//     }
//     SpawnedActors.Empty();
// }
//
// #if UE_EDITOR
// // Override PostEditChangeProperty to respond to property changes in the editor
// void ARandomMover::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
// {
//     Super::PostEditChangeProperty(PropertyChangedEvent);
//
//     // 检查是否是 NumberOfActors 或 BlueprintPath 属性发生了变化
//     if (PropertyChangedEvent.Property && 
//         (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(ARandomMover, NumberOfActors) ||
//          PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(ARandomMover, BlueprintPath)))
//     {
//         // 销毁现有的角色
//         DestroyActors();
//
//         // 重新生成角色
//         SpawnActors();
//     }
// }
//
// void ARandomMover::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
// {
//     Super::SetupPlayerInputComponent(PlayerInputComponent);
//
//     // 转换为增强输入组件
//     if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
//     {
//         // 绑定动作
//         EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ARandomMover::OnInputMoveAxis2D);
//     }
// }
//
// void ARandomMover::OnInputMoveAxis2D(const FInputActionValue& Value)
// {
//     Move_Axis2D = Value.Get<FVector2D>();
// }
// #endif
