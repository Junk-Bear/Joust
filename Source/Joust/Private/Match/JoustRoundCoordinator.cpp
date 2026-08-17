// Fill out your copyright notice in the Description page of Project Settings.


#include "Match/JoustRoundCoordinator.h"
#include "Match/JoustPhaseCoordinator.h"
#include "Interface/JoustRandomProvider.h"
#include "Rules/JoustRuleSetDataAsset.h"
#include "Result/FJoustRoundResolver.h"
#include "Match/JoustMatchCoordinator.h"
#include "Strategy/JoustStrategyService.h"

void UJoustRoundCoordinator::Initialize(UJoustPhaseCoordinator* InPhaseCoordinator, UJoustRuleSetDataAsset* InRuleSet, IJoustRandomProvider& InRandomProvider)
{
	//남은 이벤트 바인딩 제거
	if (PhaseCoordinator != nullptr)
	{
		PhaseCoordinator->OnPhaseEnded().RemoveAll(this);
	}

	PhaseCoordinator = InPhaseCoordinator;
	RuleSet = InRuleSet;
	RandomProvider = &InRandomProvider;

	RoundNumber = 0;

	bRoundActive = false;

	FlowState = ERoundFlowState::Idle;

	ResetRoundData();

	//새 PhaseCoordinator의 페이즈종료 이벤트 구독
	if (PhaseCoordinator != nullptr)
	{
		PhaseCoordinator->OnPhaseEnded().AddUObject(this, &UJoustRoundCoordinator::HandlePhaseEnded);
	}

	if (StrategyService == nullptr)
	{
		StrategyService = NewObject<UJoustStrategyService>(this);
	}
	else
	{
		StrategyService->Initialize(RuleSet, InRandomProvider);
	}
}

bool UJoustRoundCoordinator::StartRound(int32 InRoundNumber)
{
	if (PhaseCoordinator == nullptr)
		return false;

	if (RuleSet == nullptr)
		return false;

	if (RandomProvider == nullptr)
		return false;

	//라운드 진행 중이면 새라운드 시작 X
	if (bRoundActive)
		return false;

	//첫라운드 or 이전라운드가 완전히 Finished된 경우에만 가능하도록 방지
	if (FlowState != ERoundFlowState::Idle && FlowState != ERoundFlowState::Finished)
		return false;

	//PhaseCoordinator에서 이전 TimePhase가 아직 실행중이면 안됨
	if (PhaseCoordinator->IsPhaseActive())
		return false;

	if (InRoundNumber <= 0)
		return false;

	RoundNumber = InRoundNumber;

	ResetRoundData();

	bRoundActive = true;

	//Draw, Ban 등 준비 끝나면 Phase1 진행
	FlowState = ERoundFlowState::ReadyForStrategy;

	return true;
}

bool UJoustRoundCoordinator::BeginStrategyPhase()
{
	if (RuleSet == nullptr)
		return false;

	return BeginTimedPhase(
		EJoustPhase::Strategy,
		RuleSet->StrategyPhaseDuration,
		RuleSet->OnePlayerCompletedRemainingTime,
		ERoundFlowState::ReadyForStrategy, ERoundFlowState::Strategy
	);

}

bool UJoustRoundCoordinator::BeginAttackPhase()
{
	if (RuleSet == nullptr)
		return false;

	return BeginTimedPhase(
		EJoustPhase::Attack,
		RuleSet->AttackPhaseDuration,
		RuleSet->OnePlayerCompletedRemainingTime,
		ERoundFlowState::ReadyForAttack, ERoundFlowState::Attack
	);
}

bool UJoustRoundCoordinator::BeginDefensePhase(float InPlayerADefenseDuration, float InPlayerBDefenseDuration)
{
	if(InPlayerADefenseDuration <= 0.0f || InPlayerBDefenseDuration <= 0.0f)
		return false;

	//더 긴 방어시간 = 긴 제한시간
	const float DefensePhaseDuration = FMath::Max(InPlayerADefenseDuration, InPlayerBDefenseDuration);

	return BeginTimedPhase(
		EJoustPhase::Defense,
		DefensePhaseDuration,
		0.0f,
		ERoundFlowState::ReadyForDefense, ERoundFlowState::Defense
	);
}

void UJoustRoundCoordinator::MarkPlayerAComplete()
{
	if (!bRoundActive)
		return;

	if (PhaseCoordinator == nullptr)
		return;

	//계산, 완료 확인, 남은시간 단축은 PhaseCoordinator 책임
	PhaseCoordinator->MarkPlayerAComplete();
}

void UJoustRoundCoordinator::MarkPlayerBComplete()
{
	if (!bRoundActive)
		return;

	if (PhaseCoordinator == nullptr)
		return;

	//계산, 완료 확인, 남은시간 단축은 PhaseCoordinator 책임
	PhaseCoordinator->MarkPlayerBComplete();
}

bool UJoustRoundCoordinator::ResolveRound(
	const FJoustAttackData & InAAttackData, const FJoustDefenseData & InBDefenseData, float InAToBImpactTime, 
	const FJoustAttackData & InBAttackData, const FJoustDefenseData & InADefenseData, float InBToAImpactTime)
{
	if (!bRoundActive)
		return false;

	if (FlowState != ERoundFlowState::ReadyForResolve)
		return false;

	if (PhaseCoordinator == nullptr)
		return false;

	if (RuleSet != nullptr)
		return false;
	
	if (RandomProvider == nullptr)
		return false;

	//Resolver호출 전 재진입 차단
	FlowState = ERoundFlowState::Resolving;

	//Resolve는 Timer와 Player 입력이 없는 내부 계산용 Phase
	//아직 Resolver를 호출전 -> ReadyForResolve 복귀해도 안전
	if (!PhaseCoordinator->SetNoneTimedPhase(EJoustPhase::Resolve))
	{
		FlowState = ERoundFlowState::ReadyForResolve;

		return false;
	}

	//이번 라운드에서 확정된 양쪽 Attack / Defense 데이터 보관
	PlayerAAttackData = InAAttackData;
	PlayerBAttackData = InBAttackData;

	PlayerADefenseData = InADefenseData;
	PlayerBDefenseData = InBDefenseData;

	AToBImpactTime = InAToBImpactTime;
	BToAImpactTime = InBToAImpactTime;

	//실제 전투 결과 만들기
	CurrentRoundResult = FJoustRoundResolver::Reslove(
		RoundNumber,
		PlayerAAttackData, PlayerBDefenseData, AToBImpactTime,
		PlayerBAttackData, PlayerADefenseData, BToAImpactTime,	
		*RuleSet, *RandomProvider
	);

	//일어난 사건을 연출하기 위한 RoundResult Phase로 이동
	if (!PhaseCoordinator->SetNoneTimedPhase(EJoustPhase::RoundResult))
		return false;

	FlowState = ERoundFlowState::RoundResult;

	UJoustMatchCoordinator* MatchCoordinatorPtr = MatchCoordinator.Get();

	if (MatchCoordinatorPtr != nullptr)
	{
		MatchCoordinatorPtr->HandleRoundResolved(CurrentRoundResult);
	}

	//방송
	RoundResultEvent.Broadcast(CurrentRoundResult);

	return true;
}

bool UJoustRoundCoordinator::CompleteRoundResultPhase()
{
	if (!bRoundActive)
		return false;

	if (FlowState != ERoundFlowState::RoundResult)
		return false;

	FlowState = ERoundFlowState::Finished;

	bRoundActive = false;

	UJoustMatchCoordinator* MatchCoordinatorPtr = MatchCoordinator.Get();
	if (MatchCoordinator != nullptr)
	{
		MatchCoordinatorPtr->HandleRoundResolvedCompleted();
	}

	if (StrategyService != nullptr)
	{
		StrategyService->EndRound();
	}

	return true;
}

void UJoustRoundCoordinator::BeginDestroy()
{
	if (PhaseCoordinator != nullptr)
	{
		PhaseCoordinator->OnPhaseEnded().RemoveAll(this);
	}

	RandomProvider = nullptr;

	Super::BeginDestroy();
}

bool UJoustRoundCoordinator::BeginTimedPhase(
	EJoustPhase InPhase, 
	float InDurationSeconds, 
	float InShortenRemainingTime, 
	ERoundFlowState InRequiredState, ERoundFlowState InActiveState)
{
	if (!bRoundActive)
		return false;

	//올바른 Ready상태에서만 해당 Phase로 진입가능
	if (FlowState != InRequiredState)
		return false;

	if (PhaseCoordinator == nullptr)
		return false;

	const bool bStarted = PhaseCoordinator->StartTimedPhase(InPhase, InDurationSeconds, InShortenRemainingTime);

	if (!bStarted)
		return false;

	FlowState = InActiveState;

	return true;
}

void UJoustRoundCoordinator::HandlePhaseEnded(EJoustPhase EndedPhase)
{
	if (!bRoundActive)
		return;

	switch (EndedPhase)
	{

	case EJoustPhase::Strategy:
		if (FlowState == ERoundFlowState::Strategy)
		{
			FlowState = ERoundFlowState::ReadyForAttack;
		}
		break;
	
	case EJoustPhase::Attack:
		if (FlowState == ERoundFlowState::Attack)
		{
			FlowState = ERoundFlowState::ReadyForDefense;
		}
		break;

	case EJoustPhase::Defense:
		if (FlowState == ERoundFlowState::Defense)
		{
			FlowState = ERoundFlowState::ReadyForResolve;
		}
		break;

	default:
		break;
	}
}

void UJoustRoundCoordinator::ResetRoundData()
{
	PlayerAAttackData = FJoustAttackData{};
	PlayerBAttackData = FJoustAttackData{};

	PlayerADefenseData = FJoustDefenseData{};
	PlayerBDefenseData = FJoustDefenseData{};

	AToBImpactTime = 0.0f;
	BToAImpactTime = 0.0f;

	CurrentRoundResult = FJoustRoundResult{};
}
