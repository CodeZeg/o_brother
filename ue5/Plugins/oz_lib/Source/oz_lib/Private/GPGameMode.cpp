// Fill out your copyright notice in the Description page of Project Settings.


#include "GPGameMode.h"

#include "GPGameState.h"
#include "GPHUD.h"
#include "GPPawn.h"
#include "GPPlayerController.h"
#include "GPPlayerState.h"
#include "GPSpectator.h"

AGPGameMode::AGPGameMode()
{
	DefaultPawnClass = AGPPawn::StaticClass();
	HUDClass = AGPHUD::StaticClass();
	PlayerControllerClass = AGPPlayerController::StaticClass();
	GameStateClass = AGPGameState::StaticClass();
	PlayerStateClass = AGPPlayerState::StaticClass();
	SpectatorClass = AGPSpectator::StaticClass();
}

void AGPGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
}
