// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/JoustGameMode.h"
#include "Framework/JoustGameState.h"
#include "Framework/JoustPlayerState.h"
#include "Framework/JoustPlayerController.h"
#include "Framework/JoustPawn.h"

AJoustGameMode::AJoustGameMode()
{
	GameStateClass = AJoustGameState::StaticClass();
	PlayerStateClass = AJoustPlayerState::StaticClass();
	PlayerControllerClass = AJoustPlayerController::StaticClass();
	DefaultPawnClass = AJoustPawn::StaticClass();
}
