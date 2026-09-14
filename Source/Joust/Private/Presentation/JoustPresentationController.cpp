// Fill out your copyright notice in the Description page of Project Settings.


#include "Presentation/JoustPresentationController.h"
#include "Framework/JoustGameState.h"
#include "Framework/JoustPlayerController.h"
#include "Framework/JoustPlayerState.h"
#include "Presentation/JoustHUD.h"
#include "Presentation/JoustHUDWidget.h"
#include "Engine/World.h"
#include "Presentation/JoustMatchStartWidget.h"
#include "Presentation/JoustStrategyWidget.h"
#include "Rules/JoustRuleSetDataAsset.h"

bool UJoustPresentationController::Initialize(AJoustPlayerController* InPlayerController, AJoustHUD* InHUD, UJoustHUDWidget* InRootWidget)
{
	if (!IsValid(InPlayerController) || !IsValid(InHUD) || !IsValid(InRootWidget) || !InPlayerController->IsLocalController())
		return false;
	
	UWorld* WorldPtr = InPlayerController->GetWorld();

	if (!IsValid(WorldPtr))
		return false;

	AJoustGameState* JoustGameStatePtr = WorldPtr->GetGameState<AJoustGameState>();

	if (!IsValid(JoustGameStatePtr))
		return false;

	StopTimerUpdates();
	UnbindWidgetEvents();
	UnbindGameStateEvents();
	
	PlayerController = InPlayerController;
	GameState = JoustGameStatePtr;
	HUD = InHUD;
	RootWidget = InRootWidget;

	BindGameStateEvents();
	BindWidgetEvents();

	RefreshCommonHUD();
	RefreshCurrentScreen();
	RefreshStrategyScreen();

	LastDisplayedRemainingSeconds = INDEX_NONE;
	UpdateRemainingTime();
	StartTimerUpdates();

	return true;
}

AJoustPlayerState* UJoustPresentationController::GetPlayerState() const
{
	AJoustPlayerController* PlayerControllerPtr = PlayerController.Get();

	if (!IsValid(PlayerControllerPtr))
		return nullptr;
	
	return PlayerControllerPtr->GetPlayerState<AJoustPlayerState>();
}

void UJoustPresentationController::BeginDestroy()
{
	StopTimerUpdates();
	UnbindWidgetEvents();
	UnbindGameStateEvents();

	RootWidget.Reset();
	HUD.Reset();
	GameState.Reset();
	PlayerController.Reset();

	Super::BeginDestroy();
}

void UJoustPresentationController::BindGameStateEvents()
{
	AJoustGameState* GameStatePtr = GameState.Get();

	if (!IsValid(GameStatePtr))
		return;

	GameStatePtr->OnMatchStateChanged().AddUObject(this, &UJoustPresentationController::HandleMatchStateChanged);
	GameStatePtr->OnPhaseStateChanged().AddUObject(this, &UJoustPresentationController::HandlePhaseStateChanged);
	GameStatePtr->OnRoundResultChanged().AddUObject(this, &UJoustPresentationController::HandleRoundResultChanged);
	GameStatePtr->OnMatchResultChanged().AddUObject(this, &UJoustPresentationController::HandleMatchResultChanged);
	GameStatePtr->OnStrategyStateChanged().AddUObject(this, &UJoustPresentationController::HandleStrategyStateChanged);
}

void UJoustPresentationController::UnbindGameStateEvents()
{
	AJoustGameState* GameStatePtr = GameState.Get();

	if (!GameState.IsValid())
		return;

	GameStatePtr->OnMatchStateChanged().RemoveAll(this);
	GameStatePtr->OnPhaseStateChanged().RemoveAll(this);
	GameStatePtr->OnRoundResultChanged().RemoveAll(this);
	GameStatePtr->OnMatchResultChanged().RemoveAll(this);
	GameStatePtr->OnStrategyStateChanged().RemoveAll(this);
}

void UJoustPresentationController::HandleMatchStateChanged()
{
	RefreshCommonHUD();

	RefreshCurrentScreen();

	RefreshStrategyScreen();
}

void UJoustPresentationController::HandlePhaseStateChanged()
{
	RefreshCommonHUD();

	RefreshCurrentScreen();

	AJoustGameState* GameStatePtr = GameState.Get();

	if (IsValid(GameStatePtr) && GameStatePtr->GetCurrentPhase() == EJoustPhase::Strategy)
	{
		RefreshStrategyScreen();
	}

	LastDisplayedRemainingSeconds = INDEX_NONE;

	UpdateRemainingTime();
}

void UJoustPresentationController::HandleRoundResultChanged()
{
	RefreshCurrentScreen();
}

void UJoustPresentationController::HandleMatchResultChanged()
{
	RefreshCurrentScreen();
}

void UJoustPresentationController::RefreshCommonHUD()
{
	AJoustGameState* GameStatePtr = GameState.Get();
	UJoustHUDWidget* RootWidgetPtr = RootWidget.Get();

	if (!IsValid(GameStatePtr) || !IsValid(RootWidgetPtr))
		return;

	RootWidgetPtr->UpdateCommonHUD(
		GameStatePtr->GetCurrentRoundNumber(),
		GameStatePtr->GetPlayerAScore(),
		GameStatePtr->GetPlayerBScore(),
		GameStatePtr->GetCurrentPhase()
	);
}

void UJoustPresentationController::RefreshCurrentScreen()
{
	AJoustGameState* GameStatePtr = GameState.Get();
	UJoustHUDWidget* RootWidgetPtr = RootWidget.Get();

	if (!IsValid(GameStatePtr) || !IsValid(RootWidgetPtr))
		return;

	if (GameStatePtr->GetCurrentRoundNumber() <= 0 && !GameStatePtr->IsMatchFinished())
	{
		RootWidgetPtr->ShowMatchStart();
		
		return;
	}

	switch (GameStatePtr->GetCurrentPhase())
	{
	case EJoustPhase::RoundResult:
	{
		const FJoustRoundResult& RoundResultRef = GameStatePtr->GetLastRoundResult();

		if (RoundResultRef.RoundNumber != GameStatePtr->GetCurrentRoundNumber())
			return;

		RootWidgetPtr->ShowPhase(EJoustPhase::RoundResult, RoundResultRef.AtoBExchangeResult.bDefenderUnhorsed || RoundResultRef.BtoAExchangeResult.bDefenderUnhorsed);

		break;
	}

	case EJoustPhase::MatchResult:
		if (!GameStatePtr->IsMatchFinished())
			return;
		
		RootWidgetPtr->ShowPhase(EJoustPhase::MatchResult, false);
		break;

	default:
		RootWidgetPtr->ShowPhase(GameStatePtr->GetCurrentPhase(), false);
		
		break;
	}
}

void UJoustPresentationController::StartTimerUpdates()
{
	StopTimerUpdates();

	AJoustPlayerController* PlayerControllerPtr = PlayerController.Get();

	if (!IsValid(PlayerControllerPtr))
		return;
	
	UWorld* WorldPtr = PlayerControllerPtr->GetWorld();

	if (!IsValid(WorldPtr))
		return;
	
	WorldPtr->GetTimerManager().SetTimer(TimerUpdateHandle, this, &UJoustPresentationController::UpdateRemainingTime, 0.1f, true);
}

void UJoustPresentationController::StopTimerUpdates()
{
	AJoustPlayerController* PlayerControllerPtr = PlayerController.Get();

	if (IsValid(PlayerControllerPtr))
	{
		UWorld* WorldPtr = PlayerControllerPtr->GetWorld();

		if (IsValid(WorldPtr))
		{
			WorldPtr->GetTimerManager().ClearTimer(TimerUpdateHandle);
		}
	}

	TimerUpdateHandle.Invalidate();
}

void UJoustPresentationController::UpdateRemainingTime()
{
	AJoustGameState* GameStatePtr = GameState.Get();

	UJoustHUDWidget* RootWidgetPtr = RootWidget.Get();

	if (!IsValid(GameStatePtr) || !IsValid(RootWidgetPtr))
		return;
	
	int32 RemainingSeconds = 0;

	switch (GameStatePtr->GetCurrentPhase())
	{
	case EJoustPhase::Strategy:
	case EJoustPhase::Attack:
	case EJoustPhase::Defense:
	{
		const float RemainingTime = FMath::Max(0.0f, GameStatePtr->GetPhaseEndTime() - static_cast<float>(GameStatePtr->GetServerWorldTimeSeconds()));

		RemainingSeconds = FMath::CeilToInt(RemainingTime);

		break;
	}

	default:
		break;
	}

	if (RemainingSeconds == LastDisplayedRemainingSeconds)
		return;
	
	LastDisplayedRemainingSeconds = RemainingSeconds;

	RootWidgetPtr->UpdateRemainingTime(RemainingSeconds);
}

void UJoustPresentationController::HandleStrategyStateChanged()
{
	RefreshStrategyScreen();
}

void UJoustPresentationController::RefreshStrategyScreen()
{
	AJoustGameState* GameStatePtr = GameState.Get();

	UJoustHUDWidget* HUDWidgetPtr = RootWidget.Get();

	if (!IsValid(GameStatePtr) || !IsValid(HUDWidgetPtr))
		return;

	UJoustStrategyWidget* StrategyWidgetPtr = HUDWidgetPtr->GetStrategyWidget();

	if (!IsValid(StrategyWidgetPtr))
		return;

	const UJoustRuleSetDataAsset* RuleSetPtr = GameStatePtr->GetRuleSet();

	if (!IsValid(RuleSetPtr))
		return;

	StrategyWidgetPtr->SetStrategyCards(GameStatePtr->GetPublicStrategyCardIDs(), RuleSetPtr);

	AJoustPlayerState* PlayerStatePtr = GetPlayerState();

	if (!IsValid(PlayerStatePtr))
		return;

	const bool bLocalPlayerA = PlayerStatePtr == GameStatePtr->GetPlayerAState();
	const bool bLocalPlayerB = PlayerStatePtr == GameStatePtr->GetPlayerBState();

	if (!bLocalPlayerA && !bLocalPlayerB)
		return;

	bool bLocalBanPending = false;

	FName BannedCardIDForPlayer = NAME_None;

	if (bLocalPlayerA)
	{
		bLocalBanPending = GameStatePtr->IsPlayerAStrategyBanPending();

		BannedCardIDForPlayer = GameStatePtr->GetBannedCardIDForPlayerA();
	}
	else
	{
		bLocalBanPending = GameStatePtr->IsPlayerBStrategyBanPending();

		BannedCardIDForPlayer = GameStatePtr->GetBannedCardIDForPlayerB();
	}

	const bool bAnyBanPending = GameStatePtr->IsPlayerAStrategyBanPending() || GameStatePtr->IsPlayerBStrategyBanPending();

	StrategyWidgetPtr->SetStrategyInteractionState(bLocalBanPending, bAnyBanPending, BannedCardIDForPlayer);

	StrategyWidgetPtr->SetHistoryData(PlayerStatePtr->GetAttackHistory(), PlayerStatePtr->GetDefenseHistory(), RuleSetPtr->LanceBoxMin, RuleSetPtr->LanceBoxMax);
}

void UJoustPresentationController::HandleStrategyConfirmed(FName InCardID)
{
	AJoustPlayerController* PlayerControllerPtr = PlayerController.Get();

	if (!IsValid(PlayerControllerPtr))
		return;

	PlayerControllerPtr->RequestStrategySelection(InCardID);
}

void UJoustPresentationController::HandleStrategyBanConfirmed(FName InCardID)
{
	AJoustPlayerController* PlayerControllerPtr = PlayerController.Get();

	if (!IsValid(PlayerControllerPtr))
		return;

	PlayerControllerPtr->RequestStrategyBan(InCardID);
}

void UJoustPresentationController::BindWidgetEvents()
{
	UJoustHUDWidget* RootWidgetPtr = RootWidget.Get();

	if (!IsValid(RootWidgetPtr))
		return;

	UJoustMatchStartWidget* MatchStartWidgetPtr = RootWidgetPtr->GetMatchStartWidget();

	if (!IsValid(MatchStartWidgetPtr))
		return;

	MatchStartWidgetPtr->OnStartMatchRequested().AddUObject(this, &UJoustPresentationController::HandleStartMatchRequested);

	UJoustStrategyWidget* StrategyWidgetPtr = RootWidgetPtr->GetStrategyWidget();

	if (IsValid(StrategyWidgetPtr))
	{
		StrategyWidgetPtr->OnStrategyConfirmed().AddUObject(this, &UJoustPresentationController::HandleStrategyConfirmed);

		StrategyWidgetPtr->OnStrategyBanConfirmed().AddUObject(this, &UJoustPresentationController::HandleStrategyBanConfirmed);
	}
}

void UJoustPresentationController::UnbindWidgetEvents()
{
	UJoustHUDWidget* RootWidgetPtr = RootWidget.Get();

	if (!IsValid(RootWidgetPtr))
		return;

	UJoustMatchStartWidget* MatchStartWidgetPtr = RootWidgetPtr->GetMatchStartWidget();

	if (IsValid(MatchStartWidgetPtr))
	{
		MatchStartWidgetPtr->OnStartMatchRequested().RemoveAll(this);
	}

	UJoustStrategyWidget* StrategyWidgetPtr = RootWidgetPtr->GetStrategyWidget();

	if (IsValid(StrategyWidgetPtr))
	{
		StrategyWidgetPtr->OnStrategyConfirmed().RemoveAll(this);

		StrategyWidgetPtr->OnStrategyBanConfirmed().RemoveAll(this);
	}
}

void UJoustPresentationController::HandleStartMatchRequested()
{
	AJoustPlayerController* PlayerControllerPtr = PlayerController.Get();

	if (!IsValid(PlayerControllerPtr))
		return;

	PlayerControllerPtr->RequestStartMatch();
}
