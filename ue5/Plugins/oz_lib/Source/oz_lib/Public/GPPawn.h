// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
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

protected:
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void OnInputMoveAxis2D(const FInputActionValue& Value);
private:
	UPROPERTY(EditAnywhere, Category = "GPPawn", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputMappingContext> IMC_Local_PLayer;
	UPROPERTY(EditAnywhere, Category = "GPPawn", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> IA_Move;
	UPROPERTY(EditAnywhere, Category = "GPPawn", meta = (AllowPrivateAccess = "true"))
	FVector2D Input_Move_Axis2D;

};
