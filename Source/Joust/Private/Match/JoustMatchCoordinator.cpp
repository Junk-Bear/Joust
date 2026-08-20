// Fill out your copyright notice in the Description page of Project Settings.


#include "Match/JoustMatchCoordinator.h"
#include "Match/JoustRoundCoordinator.h"
#include "Rules/JoustRuleSetDataAsset.h"
#include "Result/FJoustMatchResultResolver.h"
#include "Match/JoustPhaseCoordinator.h"
#include "Framework/JoustGameState.h"

void UJoustMatchCoordinator::Initialize(UJoustRoundCoordinator* InRoundCoordinator, UJoustPhaseCoordinator* InPhaseCoordinator, const UJoustRuleSetDataAsset* InRuleSet)
{
	if (RoundCoordinator != nullptr)
	{
		RoundCoordinator->SetMatchCoordinator(nullptr);
	}

	GameState.Reset();

	RoundCoordinator = InRoundCoordinator;
	PhaseCoordinator = InPhaseCoordinator;

	//룰셋 파라미터에서 꺼내기
	BaseRoundCount = (InRuleSet != nullptr) ? InRuleSet->BaseRoundCount : 0;

	ResetMatchData();

	FlowState = EMatchFlowState::Idle;

	if (RoundCoordinator != nullptr)
	{
		RoundCoordinator->SetMatchCoordinator(this);
	}
}

bool UJoustMatchCoordinator::StartMatch()
{
	if (RoundCoordinator == nullptr || !PhaseCoordinator.IsValid() || BaseRoundCount <= 0)
		return false;

	if (FlowState != EMatchFlowState::Idle && FlowState != EMatchFlowState::Finished)
		return false;

	if (RoundCoordinator->IsRoundActive())
		return false;

	ResetMatchData();

	AJoustGameState* GameStatePtr = GameState.Get();

	if (GameStatePtr != nullptr)
	{
		GameStatePtr->ResetMatchState();
	}

	CurrentRoundNumber = 1;

	FlowState = EMatchFlowState::ReadyForRound;

	if (!StartCurrentRound())
	{
		CurrentRoundNumber = 0;

		FlowState = EMatchFlowState::Idle;

		return false;
	}

	return true;
}

bool UJoustMatchCoordinator::CompleteMatchResultPhase()
{
	if (FlowState != EMatchFlowState::MatchResult)
		return false;

	FlowState = EMatchFlowState::Finished;
	
	return true;
}

void UJoustMatchCoordinator::BeginDestroy()
{
	if (RoundCoordinator != nullptr)
	{
		RoundCoordinator->SetMatchCoordinator(nullptr);
	}
	
	GameState.Reset();

	PhaseCoordinator.Reset();


	Super::BeginDestroy();
}

bool UJoustMatchCoordinator::StartCurrentRound()
{
	if (
		FlowState != EMatchFlowState::ReadyForRound ||
		RoundCoordinator == nullptr || 
		CurrentRoundNumber <= 0 || 
		!RoundCoordinator->StartRound(CurrentRoundNumber))
		return false;

	FlowState = EMatchFlowState::RoundInProgress;

	AJoustGameState* GameStatePtr = GameState.Get();

	if (GameStatePtr != nullptr)
	{
		GameStatePtr->SetCurrentRoundNumber(CurrentRoundNumber);
	}

	return true;
}

void UJoustMatchCoordinator::HandleRoundResolved(FJoustRoundResult& RoundResult)
{
	if (FlowState != EMatchFlowState::RoundInProgress)
		return;

	if (RoundResult.RoundNumber != CurrentRoundNumber)
		return;

	PlayerAScore += RoundResult.AtoBExchangeResult.ScoreDelta;
	PlayerBScore += RoundResult.BtoAExchangeResult.ScoreDelta;

	AJoustGameState* GameStatePtr = GameState.Get();

	if (GameStatePtr != nullptr)
	{
		GameStatePtr->SetScores(PlayerAScore, PlayerBScore);
	}

	const bool bPlayerAUnhorsed = RoundResult.BtoAExchangeResult.bDefenderUnhorsed;
	const bool bPlayerBUnhorsed = RoundResult.AtoBExchangeResult.bDefenderUnhorsed;

	//MatchResult 만들게 시키기
	CurrentMatchResult = FJoustMatchResultResolver::Resolve(
		CurrentRoundNumber, BaseRoundCount,
		PlayerAScore, PlayerBScore,
		bPlayerAUnhorsed, bPlayerBUnhorsed
	);

	FlowState = EMatchFlowState::WaitingForRoundResultCompletion;
}

void UJoustMatchCoordinator::HandleRoundResolvedCompleted()
{
	if (FlowState != EMatchFlowState::WaitingForRoundResultCompletion)
		return;

	if (CurrentMatchResult.MatchOutcome == EJoustMatchOutcome::Undecided)
	{
		CurrentRoundNumber++;

		FlowState = EMatchFlowState::ReadyForRound;

		StartCurrentRound();

		return;
	}

	UJoustPhaseCoordinator* PhaseCoordinatorPtr = PhaseCoordinator.Get();

	if (PhaseCoordinatorPtr == nullptr)
		return;

	if (!PhaseCoordinatorPtr->SetNoneTimedPhase(EJoustPhase::MatchResult))
		return;

	AJoustGameState* GameStatePtr = GameState.Get();

	if (GameStatePtr != nullptr)
	{
		GameStatePtr->SetPhaseState(EJoustPhase::MatchResult, PhaseCoordinatorPtr->GetPhaseEndTime());

		GameStatePtr->SetMatchResult(CurrentMatchResult);
	}

	FlowState = EMatchFlowState::MatchResult;

	MatchResultEvent.Broadcast(CurrentMatchResult);
}

void UJoustMatchCoordinator::ResetMatchData()
{
	CurrentRoundNumber = 0;

	PlayerAScore = 0;
	PlayerBScore = 0;

	CurrentMatchResult = FJoustMatchResult{};
}
