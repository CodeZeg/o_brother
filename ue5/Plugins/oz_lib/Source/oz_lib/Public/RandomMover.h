// #pragma once
//
// #include "CoreMinimal.h"
// #include "InputMappingContext.h"
// #include "GameFramework/Actor.h"
// #include "RandomMover.generated.h"
//
//
// UCLASS()
// class OZ_LIB_API ARandomMover : public APawn
// {
// 	GENERATED_BODY()
//
// public:
// 	// Sets default values for this actor's properties
// 	ARandomMover();
//
// protected:
// 	// Called when the game starts or when spawned
// 	virtual void BeginPlay() override;
//
// 	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
//
// public:
// 	// Called every frame
// 	virtual void Tick(float DeltaTime) override;
//
// private:
// 	UPROPERTY(EditAnywhere, Category = "RandomMover", meta = (AllowPrivateAccess = "true"))
// 	UInputMappingContext* IMC_Local_PLayer;
// 	UPROPERTY(EditAnywhere, Category = "RandomMover", meta = (AllowPrivateAccess = "true"))
// 	UInputAction* IA_Move;
// 	UPROPERTY(EditAnywhere, Category = "RandomMover", meta = (AllowPrivateAccess = "true"))
// 	FVector2D Move_Axis2D;
// 	
// 	// Number of actors to spawn
// 	UPROPERTY(EditAnywhere, Category = "RandomMover", meta = (AllowPrivateAccess = "true"))
// 	int32 NumberOfActors;
//
// 	// Array to hold references to spawned actors
// 	UPROPERTY()
// 	TArray<AActor*> SpawnedActors;
//
// 	// Movement speed multiplier
// 	UPROPERTY(EditAnywhere, Category = "RandomMover", meta = (AllowPrivateAccess = "true"))
// 	float MovementSpeed;
//
// 	// Blueprint class path as a string
// 	UPROPERTY(EditAnywhere, Category = "RandomMover", meta = (AllowPrivateAccess = "true"))
// 	FString BlueprintPath;
//
// 	// Function to change directions randomly
// 	void ChangeDirections();
//
// 	// Function to spawn actors
// 	void SpawnActors();
// 	void OnBlueprintLoaded();
//
// 	// Function to destroy all spawned actors
// 	void DestroyActors();
//
// #if UE_EDITOR
// 	// Override PostEditChangeProperty to respond to property changes in the editor
// 	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
//
// #endif
// 	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
// 	void OnInputMoveAxis2D(const FInputActionValue& Value);
// };