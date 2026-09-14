// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/JoustGameState.h"
#include "Net/UnrealNetwork.h"
#include "Framework/JoustPlayerState.h"

void AJoustGameState::ResetMatchState()
{
	CurrentRoundNumber = 0;

	CurrentPhase = EJoustPhase::Strategy;

	PhaseEndTime = 0.0f;

	PlayerAScore = 0;
	PlayerBScore = 0;

	PublicStrategyCardIDs.Reset();
	BannedCardIDForPlayerA = NAME_None;
	BannedCardIDForPlayerB = NAME_None;

	LastRoundResult = FJoustRoundResult{};
	
	CurrentMatchResult = FJoustMatchResult{};

	bMatchFinished = false;

	MatchStateChangedEvent.Broadcast();
	PhaseStateChangedEvent.Broadcast();
	StrategyStateChangedEvent.Broadcast();
	RoundResultChangedEvent.Broadcast();
	MatchResultChangedEvent.Broadcast();
}

void AJoustGameState::SetPhaseState(EJoustPhase InPhase, float InPhaseEndTime)
{
	CurrentPhase = InPhase;

	PhaseEndTime = InPhaseEndTime;

	PhaseStateChangedEvent.Broadcast();
}

void AJoustGameState::SetScores(int32 InPlayerAScore, int32 InPlayerBScore)
{
	PlayerAScore = InPlayerAScore;
	PlayerBScore = InPlayerBScore;

	MatchResultChangedEvent.Broadcast();
}

void AJoustGameState::SetPublicStrategyCardIDs(const TArray<FName>& InCardIDs)
{
	PublicStrategyCardIDs = InCardIDs;

	StrategyStateChangedEvent.Broadcast();
}

void AJoustGameState::ClearStrategyState()
{
	PublicStrategyCardIDs.Reset();

	bPlayerAStrategyBanPending = false;
	bPlayerBStrategyBanPending = false;

	BannedCardIDForPlayerA = NAME_None;
	BannedCardIDForPlayerB = NAME_None;

	StrategyStateChangedEvent.Broadcast();
}

void AJoustGameState::SetLastRoundResult(const FJoustRoundResult& InRoundResult)
{
	LastRoundResult = InRoundResult;

	RoundResultChangedEvent.Broadcast();
}

void AJoustGameState::SetMatchResult(const FJoustMatchResult& InMatchResult)
{
	CurrentMatchResult = InMatchResult;

	bMatchFinished = true;

	MatchResultChangedEvent.Broadcast();
}

void AJoustGameState::SetCurrentRoundNumber(int32 InRoundNumber)
{
	CurrentRoundNumber = InRoundNumber;

	MatchStateChangedEvent.Broadcast();
}

void AJoustGameState::SetRuleSet(UJoustRuleSetDataAsset* InRuleSet)
{ 
	RuleSet = InRuleSet;

	StrategyStateChangedEvent.Broadcast();
}

void AJoustGameState::SetParticipantStates(AJoustPlayerState* InPlayerAState, AJoustPlayerState* InPlayerBState)
{
	PlayerAState = InPlayerAState;
	PlayerBState = InPlayerBState;

	MatchStateChangedEvent.Broadcast();
}

void AJoustGameState::SetPendingStrategyBans(bool bInPlayerAPendingBan, bool bInPlayerBPendingBan)
{
	bPlayerAStrategyBanPending = bInPlayerAPendingBan;
	bPlayerBStrategyBanPending = bInPlayerBPendingBan;

	StrategyStateChangedEvent.Broadcast();
}

void AJoustGameState::ApplyStrategyBan(bool bInBanningPlayerA, FName InCardID)
{
	if (InCardID.IsNone())
		return;

	if (bInBanningPlayerA)
	{
		BannedCardIDForPlayerB = InCardID;
		bPlayerAStrategyBanPending = false;
	}
	else
	{
		BannedCardIDForPlayerA = InCardID;
		bPlayerBStrategyBanPending = false;
	}

	StrategyStateChangedEvent.Broadcast();
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

	DOREPLIFETIME(AJoustGameState, RuleSet);

	DOREPLIFETIME(AJoustGameState, PlayerAState);

	DOREPLIFETIME(AJoustGameState, PlayerBState);

	DOREPLIFETIME(AJoustGameState, bPlayerAStrategyBanPending);

	DOREPLIFETIME(AJoustGameState, bPlayerBStrategyBanPending);
}

void AJoustGameState::OnRep_MatchState()
{
	MatchStateChangedEvent.Broadcast();
}

void AJoustGameState::OnRep_PhaseState()
{
	PhaseStateChangedEvent.Broadcast();
}

void AJoustGameState::OnRep_StrategyState()
{
	StrategyStateChangedEvent.Broadcast();
}

void AJoustGameState::OnRep_RoundResult()
{
	RoundResultChangedEvent.Broadcast();
}

void AJoustGameState::OnRep_MatchResult()
{
	MatchResultChangedEvent.Broadcast();
}
