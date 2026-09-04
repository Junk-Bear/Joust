// Fill out your copyright notice in the Description page of Project Settings.


#include "Presentation/JoustPresentationController.h"
#include "Framework/JoustGameState.h"
#include "Framework/JoustPlayerController.h"
#include "Framework/JoustPlayerState.h"
#include "Presentation/JoustHUD.h"
#include "Engine/World.h"

bool UJoustPresentationController::Initialize(AJoustPlayerController* InPlayerController, AJoustHUD* InHUD)
{
	if (InPlayerController == nullptr || InHUD == nullptr || !InPlayerController->IsLocalController())
		return false;
	
	UWorld* World = InPlayerController->GetWorld();

	if (World == nullptr)
		return false;

	AJoustGameState* JoustGameState = World->GetGameState<AJoustGameState>();

	if (JoustGameState == nullptr)
		return false;
	
	PlayerController = InPlayerController;
	GameState = JoustGameState;
	HUD = InHUD;

	return true;
}

AJoustPlayerState* UJoustPresentationController::GetPlayerState() const
{
	AJoustPlayerController* PlayerControllerPtr = PlayerController.Get();

	if (PlayerControllerPtr == nullptr)
		return nullptr;
	
	return PlayerControllerPtr->GetPlayerState<AJoustPlayerState>();
}
