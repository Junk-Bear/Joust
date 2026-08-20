// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/JoustGameState.h"

#include "Net/UnrealNetwork.h"

void AJoustGameState::ResetMatchState()
{
	CurrentRoundNumber = 0;

	CurrentPhase = EJoustPhase::Strategy;

	PhaseEndTime = 0.0f;

	PlayerAScore = 0;
	PlayerBScore = 0;

	ClearStrategyState();

	LastRoundResult = FJoustRoundResult{};
	
	CurrentMatchResult = FJoustMatchResult{};

	bMatchFinished = false;
}

void AJoustGameState::SetPhaseState(EJoustPhase InPhase, float InPhaseEndTime)
{
	CurrentPhase = InPhase;

	PhaseEndTime = InPhaseEndTime;
}

void AJoustGameState::SetScores(int32 InPlayerAScore, int32 InPlayerBScore)
{
	PlayerAScore = InPlayerAScore;
	PlayerBScore = InPlayerBScore;
}

void AJoustGameState::SetPublicStrategyCardIDs(const TArray<FName>& InCardIDs)
{
	PublicStrategyCardIDs = InCardIDs;
}

void AJoustGameState::ClearStrategyState()
{
	PublicStrategyCardIDs.Reset();

	BannedCardIDForPlayerA = NAME_None;
	BannedCardIDForPlayerB = NAME_None;
}

void AJoustGameState::SetBannedCardIDForPlayer(bool bTargetPlayerA, FName InCardID)
{
	if (bTargetPlayerA)
	{
		BannedCardIDForPlayerA = InCardID;
	}
	else
	{
		BannedCardIDForPlayerB = InCardID;
	}
}

void AJoustGameState::SetLastRoundResult(const FJoustRoundResult& InRoundResult)
{
	LastRoundResult = InRoundResult;
}

void AJoustGameState::SetMatchResult(const FJoustMatchResult& InMatchResult)
{
	CurrentMatchResult = InMatchResult;

	bMatchFinished = true;
}

void AJoustGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AJoustGameState, CurrentRoundNumber);

	DOREPLIFETIME(AJoustGameState, CurrentPhase);

	DOREPLIFETIME(AJoustGameState, PhaseEndTime);

	DOREPLIFETIME(AJoustGameState, PlayerAScore);

	DOREPLIFETIME(AJoustGameState, PlayerBScore);

	DOREPLIFETIME(AJoustGameState, PublicStrategyCardIDs);

	DOREPLIFETIME(AJoustGameState, BannedCardIDForPlayerA);

	DOREPLIFETIME(AJoustGameState, BannedCardIDForPlayerB);

	DOREPLIFETIME(AJoustGameState, LastRoundResult);

	DOREPLIFETIME(AJoustGameState, CurrentMatchResult);

	DOREPLIFETIME(AJoustGameState, bMatchFinished);
}