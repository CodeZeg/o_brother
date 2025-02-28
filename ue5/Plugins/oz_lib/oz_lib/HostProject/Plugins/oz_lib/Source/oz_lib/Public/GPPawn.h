// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "input_data_generated.h"
#include "RenderDataWrapper.h"
#include "GameFramework/Pawn.h"
#include "GPPawn.generated.h"

UCLASS()
class OZ_LIB_API AGPPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGPPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void SetupLocalInput();
	void OnInputMoveAxis2D(const FInputActionValue& Value);
	flatbuffers::Offset<GPInputData> GetLocalInputData(flatbuffers::FlatBufferBuilder &builder) const;
	void ApplyRenderData(const GPRenderData* render_data);
	void SpawnActor(const GPRenderCharacterData* character);
	
	TObjectPtr<AActor> LocalActor;			// Local player actor
	TMap<int32, TObjectPtr<AActor>> SpawnedActors; // All spawned actors
private:
	UPROPERTY(EditAnywhere, Category = "GPPawn", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> IMC_Local_PLayer;
	UPROPERTY(EditAnywhere, Category = "GPPawn", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> IA_Move;
	UPROPERTY(EditAnywhere, Category = "GPPawn", meta = (AllowPrivateAccess = "true"))
	FVector2D Input_Move_Axis2D;

private:
	// 非序列化字段
	int Render_Generation;
};
