// Fill out your copyright notice in the Description page of Project Settings.


#include "Match/JoustMatchCoordinator.h"
#include "Match/JoustRoundCoordinator.h"
#include "Rules/JoustRuleSetDataAsset.h"
#include "Result/FJoustMatchResultResolver.h"
#include "Match/JoustPhaseCoordinator.h"
#include "Framework/JoustGameState.h"

void UJoustMatchCoordinator::Initialize(UJoustRoundCoordinator* InRoundCoordinator, UJoustPhaseCoordinator* InPhaseCoordinator, const UJoustRuleSetDataAsset* InRuleSet)
{
	if (IsValid(RoundCoordinator))
	{
		RoundCoordinator->SetMatchCoordinator(nullptr);
	}

	GameState.Reset();

	RoundCoordinator = InRoundCoordinator;
	PhaseCoordinator = InPhaseCoordinator;

	//룰셋 파라미터에서 꺼내기
	BaseRoundCount = (IsValid(InRuleSet)) ? InRuleSet->BaseRoundCount : 0;

	ResetMatchData();

	FlowState = EMatchFlowState::Idle;

	if (IsValid(RoundCoordinator))
	{
		RoundCoordinator->SetMatchCoordinator(this);
	}
}

bool UJoustMatchCoordinator::StartMatch()
{
	if (!IsValid(RoundCoordinator) || !PhaseCoordinator.IsValid() || BaseRoundCount <= 0)
		return false;

	if (FlowState != EMatchFlowState::Idle && FlowState != EMatchFlowState::Finished)
		return false;

	if (RoundCoordinator->IsRoundActive())
		return false;

	ResetMatchData();

	AJoustGameState* GameStatePtr = GameState.Get();

	if (IsValid(GameStatePtr))
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

bool UJoustMatchCoordinator::IsMatchActive() const
{
	return FlowState != EMatchFlowState::Idle && FlowState != EMatchFlowState::Finished;
}

void UJoustMatchCoordinator::BeginDestroy()
{
	if (IsValid(RoundCoordinator))
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
		!IsValid(RoundCoordinator) || 
		CurrentRoundNumber <= 0 || 
		!RoundCoordinator->StartRound(CurrentRoundNumber))
		return false;

	FlowState = EMatchFlowState::RoundInProgress;

	AJoustGameState* GameStatePtr = GameState.Get();

	if (IsValid(GameStatePtr))
	{
		GameStatePtr->SetCurrentRoundNumber(CurrentRoundNumber);
	}

	return true;
}

void UJoustMatchCoordinator::HandleRoundResolved(FJoustRoundResult& InRoundResult)
{
	if (FlowState != EMatchFlowState::RoundInProgress)
		return;

	if (InRoundResult.RoundNumber != CurrentRoundNumber)
		return;

	PlayerAScore += InRoundResult.AtoBExchangeResult.ScoreDelta;
	PlayerBScore += InRoundResult.BtoAExchangeResult.ScoreDelta;

	AJoustGameState* GameStatePtr = GameState.Get();

	if (IsValid(GameStatePtr))
	{
		GameStatePtr->SetScores(PlayerAScore, PlayerBScore);
	}

	const bool bPlayerAUnhorsed = InRoundResult.BtoAExchangeResult.bDefenderUnhorsed;
	const bool bPlayerBUnhorsed = InRoundResult.AtoBExchangeResult.bDefenderUnhorsed;

	//MatchResult 만들게 시키기
	CurrentMatchResult = FJoustMatchResultResolver::Resolve(
		CurrentRoundNumber, BaseRoundCount,
		PlayerAScore, PlayerBScore,
		bPlayerAUnhorsed, bPlayerBUnhorsed
	);

	FlowState = EMatchFlowState::WaitingForRoundResultCompletion;
}

bool UJoustMatchCoordinator::HandleRoundResolvedCompleted()
{
	if (FlowState != EMatchFlowState::WaitingForRoundResultCompletion)
		return false;

	if (CurrentMatchResult.MatchOutcome == EJoustMatchOutcome::Undecided)
	{
		CurrentRoundNumber++;

		FlowState = EMatchFlowState::ReadyForRound;		

		return StartCurrentRound();
	}

	UJoustPhaseCoordinator* PhaseCoordinatorPtr = PhaseCoordinator.Get();

	if (!IsValid(PhaseCoordinatorPtr))
		return false;

	if (!PhaseCoordinatorPtr->SetNoneTimedPhase(EJoustPhase::MatchResult))
		return false;

	AJoustGameState* GameStatePtr = GameState.Get();

	if (IsValid(GameStatePtr))
	{
		GameStatePtr->SetPhaseState(EJoustPhase::MatchResult, PhaseCoordinatorPtr->GetPhaseEndTime());

		GameStatePtr->SetMatchResult(CurrentMatchResult);
	}

	FlowState = EMatchFlowState::MatchResult;

	MatchResultEvent.Broadcast(CurrentMatchResult);

	return true;
}

void UJoustMatchCoordinator::ResetMatchData()
{
	CurrentRoundNumber = 0;

	PlayerAScore = 0;
	PlayerBScore = 0;

	CurrentMatchResult = FJoustMatchResult{};
}
