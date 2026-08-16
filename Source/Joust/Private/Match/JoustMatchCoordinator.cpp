// Fill out your copyright notice in the Description page of Project Settings.


#include "Match/JoustMatchCoordinator.h"
#include "Match/JoustRoundCoordinator.h"
#include "Rules/JoustRuleSetDataAsset.h"
#include "Result/FJoustMatchResultResolver.h"
#include "Match/JoustPhaseCoordinator.h"

void UJoustMatchCoordinator::Initialize(UJoustRoundCoordinator* InRoundCoordinator, UJoustPhaseCoordinator* InPhaseCoordinator, const UJoustRuleSetDataAsset* InRuleSet)
{
	//재 초기화될 경우 : 부모연결부터 해제
	if (RoundCoordinator != nullptr)
	{
		RoundCoordinator->SetMatchCoordinator(nullptr);
	}

	RoundCoordinator = InRoundCoordinator;
	PhaseCoordinator = InPhaseCoordinator;

	//룰셋 파라미터에서 꺼내기
	BaseRoundCount = (InRuleSet != nullptr) ? InRuleSet->BaseRoundCount : 0;

	ResetMatchData();

	FlowState = EMatchFlowState::Idle;

	//RoundCoordinator에서 부모 MatchCoordinator 꽂기(Back Ref)
	if (RoundCoordinator != nullptr)
	{
		RoundCoordinator->SetMatchCoordinator(this);
	}
}

bool UJoustMatchCoordinator::StartMatch()
{
	if (RoundCoordinator == nullptr)
		return false;

	if (!PhaseCoordinator.IsValid())
		return false;

	if (BaseRoundCount <= 0)
		return false;

	//첫경기 or 이전 경기가 완전히 종료된 뒤만 시작할 수 있게
	if (FlowState != EMatchFlowState::Idle && FlowState != EMatchFlowState::Finished)
		return false;

	//RoundCoordinator가 아직 라운드 진행중이면 초기화 못하게
	if (RoundCoordinator->IsRoundActive())
		return false;

	ResetMatchData();

	CurrentRoundNumber = 1;

	FlowState = EMatchFlowState::ReadyForRound;

	//실패하면 부분롤백
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

	PhaseCoordinator.Reset();

	Super::BeginDestroy();
}

bool UJoustMatchCoordinator::StartCurrentRound()
{
	if (FlowState != EMatchFlowState::ReadyForRound)
		return false;

	if (RoundCoordinator == nullptr)
		return false;

	if (CurrentRoundNumber <= 0)
		return false;

	if (!RoundCoordinator->StartRound(CurrentRoundNumber))
		return false;

	FlowState = EMatchFlowState::RoundInProgress;

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

	//연장전 돌입
	if (CurrentMatchResult.MatchOutcome == EJoustMatchOutcome::Undecided)
	{
		CurrentRoundNumber++;

		FlowState = EMatchFlowState::ReadyForRound;

		StartCurrentRound();

		return;
	}

	//승패 나옴
	UJoustPhaseCoordinator* PhaseCoordinatorPtr = PhaseCoordinator.Get();

	if (PhaseCoordinatorPtr == nullptr)
		return;

	if (!PhaseCoordinatorPtr->SetNoneTimedPhase(EJoustPhase::MatchResult))
		return;

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
