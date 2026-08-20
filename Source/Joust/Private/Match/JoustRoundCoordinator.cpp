// Fill out your copyright notice in the Description page of Project Settings.


#include "Match/JoustRoundCoordinator.h"
#include "Match/JoustPhaseCoordinator.h"
#include "Interface/JoustRandomProvider.h"
#include "Rules/JoustRuleSetDataAsset.h"
#include "Result/FJoustRoundResolver.h"
#include "Match/JoustMatchCoordinator.h"
#include "Strategy/JoustStrategyService.h"
#include "Attack/JoustAttackService.h"
#include "Attack/JoustAttackTypes.h"
#include "Prediction/JoustPredictionService.h"
#include "Prediction/JoustPredictionSeriesController.h"
#include "Interface/JoustStrategyInput.h"
#include "Interface/JoustAttackInput.h"
#include "Interface/JoustDefenseInput.h"
#include "Framework/JoustPlayerState.h"
#include "Framework/JoustGameState.h"
#include "Strategy/JoustStrategyCardDataAsset.h"

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

	PlayerAState.Reset();
	PlayerBState.Reset();

	GameState.Reset();

	RoundNumber = 0;

	bRoundActive = false;

	FlowState = ERoundFlowState::Idle;

	ResetRoundData();

	if (PhaseCoordinator != nullptr)
	{
		PhaseCoordinator->OnPhaseEnded().AddUObject(this, &UJoustRoundCoordinator::HandlePhaseEnded);
	}

	if (StrategyService == nullptr)
	{
		StrategyService = NewObject<UJoustStrategyService>(this);
	}

	StrategyService->Initialize(RuleSet, InRandomProvider);

	if (AttackService == nullptr)
	{
		AttackService = NewObject<UJoustAttackService>(this);
	}

	AttackService->Initialize(RuleSet, InRandomProvider);

	AToBPredictionService = NewObject<UJoustPredictionService>(this);

	BToAPredictionService = NewObject<UJoustPredictionService>(this);

	if (AToBPredictionService)
	{
		AToBPredictionService->Initialize(RuleSet, RandomProvider);
	}

	if (BToAPredictionService)
	{
		BToAPredictionService->Initialize(RuleSet, RandomProvider);
	}

	AToBPredictionController = NewObject<UJoustPredictionSeriesController>(this);

	BToAPredictionController = NewObject<UJoustPredictionSeriesController>(this);

	if (AToBPredictionController != nullptr)
	{
		AToBPredictionController->Initialize();

		AToBPredictionController->OnPlaybackCompleted().AddUObject(this, &UJoustRoundCoordinator::HandlePredictionPlaybackCompleted);
	}

	if (BToAPredictionController != nullptr)
	{
		BToAPredictionController->Initialize();

		BToAPredictionController->OnPlaybackCompleted().AddUObject(this, &UJoustRoundCoordinator::HandlePredictionPlaybackCompleted);
	}
}

bool UJoustRoundCoordinator::StartRound(int32 InRoundNumber)
{
	if (PhaseCoordinator == nullptr || RuleSet == nullptr || RandomProvider == nullptr || StrategyService == nullptr)
		return false;

	if (bRoundActive)
		return false;

	if (FlowState != ERoundFlowState::Idle && FlowState != ERoundFlowState::Finished)
		return false;

	if (PhaseCoordinator->IsPhaseActive())
		return false;

	if (InRoundNumber <= 0)
		return false;

	AJoustPlayerState* PlayerAStatePtr = PlayerAState.Get();
	AJoustPlayerState* PlayerBStatePtr = PlayerBState.Get();

	if (PlayerAStatePtr == nullptr || PlayerBStatePtr == nullptr)
		return false;

	PlayerAStatePtr->ResetRoundState();
	PlayerBStatePtr->ResetRoundState();

	if (!StrategyService->PrepareRound(
		PlayerAStatePtr->GetBaseStats(), PlayerBStatePtr->GetBaseStats(),
		PlayerAStatePtr->HasBanRight(), PlayerBStatePtr->HasBanRight()))
		return false;
	
	PlayerAStatePtr->SetHasBanRight(false);
	PlayerBStatePtr->SetHasBanRight(false);

	if (AJoustGameState* GameStatePtr = GameState.Get())
	{
		GameStatePtr->ClearStrategyState();
	}

	SyncStrategyPublicCards();

	RoundNumber = InRoundNumber;

	ResetRoundData();

	bRoundActive = true;

	FlowState = ERoundFlowState::ReadyForStrategy;

	return true;
}

bool UJoustRoundCoordinator::BeginStrategyPhase()
{
	if (RuleSet == nullptr || StrategyService == nullptr || !StrategyService->IsRoundPrepared())
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
	if (RuleSet == nullptr || AttackService == nullptr || !AttackService->IsRoundPrepared())
		return false;

	const bool bStarted = BeginTimedPhase(
		EJoustPhase::Attack,
		RuleSet->AttackPhaseDuration,
		RuleSet->OnePlayerCompletedRemainingTime,
		ERoundFlowState::ReadyForAttack, ERoundFlowState::Attack
	);

	if (bStarted)
	{
		AttackService->SetSubmissionOpen(true);
	}

	return bStarted;
}

bool UJoustRoundCoordinator::BeginDefensePhase()
{
	if (!ArePredictionsPrepared() || PhaseCoordinator == nullptr || AToBPredictionService == nullptr || BToAPredictionService == nullptr)
		return false;
	
	const float AToBDuration = AToBPredictionService->GetCurrentSettings().TotalDuration;
	const float BToADuration = BToAPredictionService->GetCurrentSettings().TotalDuration;

	if (AToBDuration <= 0.0f || BToADuration <= 0.0f)
		return false;
	
	if (!StartPredictionPlayback())
		return false;

	if (!BeginTimedPhase(
		EJoustPhase::Defense,
		FMath::Max(AToBDuration, BToADuration),
		0.0f,
		ERoundFlowState::ReadyForDefense,
		ERoundFlowState::Defense))
	{
		AToBPredictionController->StopPlayback();
		BToAPredictionController->StopPlayback();

		bPredictionPlaybackCompleted = false;

		return false;
	}

	AToBImpactTime = PhaseCoordinator->GetPhaseStartTime() + AToBDuration;

	BToAImpactTime = PhaseCoordinator->GetPhaseStartTime() + BToADuration;

	return true;
}

void UJoustRoundCoordinator::MarkPlayerAComplete()
{
	if (!bRoundActive || PhaseCoordinator == nullptr)
		return;

	PhaseCoordinator->MarkPlayerAComplete();

	SyncPhasePublicState();
}

void UJoustRoundCoordinator::MarkPlayerBComplete()
{
	if (!bRoundActive || PhaseCoordinator == nullptr)
		return;

	PhaseCoordinator->MarkPlayerBComplete();

	SyncPhasePublicState();
}

bool UJoustRoundCoordinator::ResolveRound()
{
	if (!bRoundActive ||
		FlowState != ERoundFlowState::ReadyForResolve ||
		PhaseCoordinator == nullptr ||
		RuleSet == nullptr ||
		RandomProvider == nullptr)
		return false;

	AJoustPlayerState* PlayerAStatePtr = PlayerAState.Get();
	AJoustPlayerState* PlayerBStatePtr = PlayerBState.Get();

	UJoustMatchCoordinator* MatchCoordinatorPtr = MatchCoordinator.Get();

	if (PlayerAStatePtr == nullptr || PlayerBStatePtr == nullptr || MatchCoordinatorPtr == nullptr)
		return false;
	
	FlowState = ERoundFlowState::Resolving;

	if (!PhaseCoordinator->SetNoneTimedPhase(EJoustPhase::Resolve))
	{
		FlowState = ERoundFlowState::ReadyForResolve;

		return false;
	}

	SyncPhasePublicState();

	CurrentRoundResult = FJoustRoundResolver::Resolve(
		RoundNumber,
		PlayerAAttackData,
		PlayerBDefenseData,
		AToBImpactTime,
		PlayerBAttackData,
		PlayerADefenseData,
		BToAImpactTime,
		*RuleSet,
		*RandomProvider);

	if (!PhaseCoordinator->SetNoneTimedPhase(EJoustPhase::RoundResult))
		return false;

	SyncPhasePublicState();
	
	FlowState = ERoundFlowState::RoundResult;

	MatchCoordinatorPtr->HandleRoundResolved(CurrentRoundResult);

	ApplyRoundResultToPlayerStates(*PlayerAStatePtr, *PlayerBStatePtr, MatchCoordinatorPtr->GetPlayerAScore(), MatchCoordinatorPtr->GetPlayerBScore());

	if (AJoustGameState* GameStatePtr = GameState.Get())
	{
		GameStatePtr->SetLastRoundResult(CurrentRoundResult);
	}

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

	if (MatchCoordinatorPtr != nullptr)
	{
		MatchCoordinatorPtr->HandleRoundResolvedCompleted();
	}

	if (StrategyService != nullptr)
	{
		StrategyService->EndRound();
	}

	if (AttackService != nullptr)
	{
		AttackService->EndRound();
	}

	if (AToBPredictionController != nullptr)
	{
		AToBPredictionController->StopPlayback();
	}

	if (BToAPredictionController != nullptr)
	{
		BToAPredictionController->StopPlayback();
	}

	bPredictionPlaybackCompleted = false;

	if (AToBPredictionService != nullptr)
	{
		AToBPredictionService->EndRound();
	}

	if (BToAPredictionService != nullptr)
	{
		BToAPredictionService->EndRound();
	}

	return true;
}

bool UJoustRoundCoordinator::PreparePredictions()
{
	if (!bRoundActive || FlowState != ERoundFlowState::ReadyForDefense || AToBPredictionService == nullptr || BToAPredictionService == nullptr)
		return false;


	AToBPredictionService->EndRound();
	BToAPredictionService->EndRound();

	if (!AToBPredictionService->PreparePrediction(
		PlayerAAttackData.AttackType,
		PlayerAAttackData.AttackPoint,
		PlayerAAttackData.PredictionSeed,
		PlayerAAttackData.Deception,
		PlayerAAttackData.Quickness,
		PlayerBDefenseData.Reading))
	{
		AToBPredictionService->EndRound();
		BToAPredictionService->EndRound();
		return false;
	}

	if (!BToAPredictionService->PreparePrediction(
		PlayerBAttackData.AttackType,
		PlayerBAttackData.AttackPoint,
		PlayerBAttackData.PredictionSeed,
		PlayerBAttackData.Deception,
		PlayerBAttackData.Quickness,
		PlayerADefenseData.Reading))
	{
		AToBPredictionService->EndRound();
		BToAPredictionService->EndRound();
		return false;
	}

	return true;
}

bool UJoustRoundCoordinator::ArePredictionsPrepared() const
{
	return AToBPredictionService && BToAPredictionService &&
		AToBPredictionService->IsPrepared() && BToAPredictionService->IsPrepared();;
}

bool UJoustRoundCoordinator::StartPredictionPlayback()
{
	if (!bRoundActive || 
		FlowState != ERoundFlowState::ReadyForDefense ||
		AToBPredictionService == nullptr || BToAPredictionService == nullptr ||
		AToBPredictionController == nullptr || BToAPredictionController == nullptr ||
		!ArePredictionsPrepared())
		return false;

	AToBPredictionController->StopPlayback();
	BToAPredictionController->StopPlayback();

	bPredictionPlaybackCompleted = false;

	if (!AToBPredictionController->StartPlayback(
		AToBPredictionService->GetCurrentSettings(),
		AToBPredictionService->GetRealSeries(),
		AToBPredictionService->GetFakeSeries()))
	{
		AToBPredictionController->StopPlayback();
		BToAPredictionController->StopPlayback();

		return false;
	}

	if (!BToAPredictionController->StartPlayback(
		BToAPredictionService->GetCurrentSettings(),
		BToAPredictionService->GetRealSeries(),
		BToAPredictionService->GetFakeSeries()))
	{
		AToBPredictionController->StopPlayback();
		BToAPredictionController->StopPlayback();

		return false;
	}

	return true;
}

void UJoustRoundCoordinator::ApplyRoundResultToPlayerStates(
	AJoustPlayerState& PlayerAStateRef, AJoustPlayerState& PlayerBStateRef, int32 PlayerAScore, int32 PlayerBScore)
{
	const FJoustExchangeResult& AToBExchange = CurrentRoundResult.AtoBExchangeResult;
	const FJoustExchangeResult& BToAExchange = CurrentRoundResult.BtoAExchangeResult;

	FJoustAttackHistory PlayerAAttackHistory{};

	PlayerAAttackHistory.RoundNumber = CurrentRoundResult.RoundNumber;
	PlayerAAttackHistory.AttackPoint = AToBExchange.AttackData.AttackPoint;
	PlayerAAttackHistory.AttackType = AToBExchange.AttackData.AttackType;
	PlayerAAttackHistory.OpponentShieldPoint = PlayerBDefenseData.ShieldPoint;
	PlayerAAttackHistory.OpponentDefenseResult = AToBExchange.DefenseResult;
	PlayerAAttackHistory.bScored = AToBExchange.ScoreDelta > 0;
	PlayerAAttackHistory.bOpponentUnhorsed = AToBExchange.bDefenderUnhorsed;

	FJoustDefenseHistory PlayerADefenseHistory{};

	PlayerADefenseHistory.RoundNumber = CurrentRoundResult.RoundNumber;
	PlayerADefenseHistory.OpponentAttackPoint = BToAExchange.AttackData.AttackPoint;
	PlayerADefenseHistory.OpponentAttackType = BToAExchange.AttackData.AttackType;
	PlayerADefenseHistory.ShieldPoint = PlayerADefenseData.ShieldPoint;
	PlayerADefenseHistory.DefenseResult = BToAExchange.DefenseResult;
	PlayerADefenseHistory.bConcededScore = BToAExchange.ScoreDelta > 0;
	PlayerADefenseHistory.bUnhorsed = BToAExchange.bDefenderUnhorsed;

	FJoustAttackHistory PlayerBAttackHistory{};

	PlayerBAttackHistory.RoundNumber = CurrentRoundResult.RoundNumber;
	PlayerBAttackHistory.AttackPoint = BToAExchange.AttackData.AttackPoint;
	PlayerBAttackHistory.AttackType = BToAExchange.AttackData.AttackType;
	PlayerBAttackHistory.OpponentShieldPoint = PlayerADefenseData.ShieldPoint;
	PlayerBAttackHistory.OpponentDefenseResult = BToAExchange.DefenseResult;
	PlayerBAttackHistory.bScored = BToAExchange.ScoreDelta > 0;
	PlayerBAttackHistory.bOpponentUnhorsed = BToAExchange.bDefenderUnhorsed;

	FJoustDefenseHistory PlayerBDefenseHistory{};

	PlayerBDefenseHistory.RoundNumber = CurrentRoundResult.RoundNumber;
	PlayerBDefenseHistory.OpponentAttackPoint = AToBExchange.AttackData.AttackPoint;
	PlayerBDefenseHistory.OpponentAttackType = AToBExchange.AttackData.AttackType;
	PlayerBDefenseHistory.ShieldPoint = PlayerBDefenseData.ShieldPoint;
	PlayerBDefenseHistory.DefenseResult = AToBExchange.DefenseResult;
	PlayerBDefenseHistory.bConcededScore = AToBExchange.ScoreDelta > 0;
	PlayerBDefenseHistory.bUnhorsed = AToBExchange.bDefenderUnhorsed;

	PlayerAStateRef.AddAttackHistory(PlayerAAttackHistory);
	PlayerAStateRef.AddDefenseHistory(PlayerADefenseHistory);

	PlayerBStateRef.AddAttackHistory(PlayerBAttackHistory);
	PlayerBStateRef.AddDefenseHistory(PlayerBDefenseHistory);

	PlayerAStateRef.SetLastDefenseResult(BToAExchange.DefenseResult);

	PlayerBStateRef.SetLastDefenseResult(AToBExchange.DefenseResult);

	if (BToAExchange.DefenseResult.bIsEdgeParry)
	{
		PlayerAStateRef.SetHasBanRight(true);
	}

	if (AToBExchange.DefenseResult.bIsEdgeParry)
	{
		PlayerBStateRef.SetHasBanRight(true);
	}

	if (BToAExchange.bDefenderUnhorsed)
	{
		PlayerAStateRef.SetUnhorsed(true);
	}

	if (AToBExchange.bDefenderUnhorsed)
	{
		PlayerBStateRef.SetUnhorsed(true);
	}

	PlayerAStateRef.SetScore(PlayerAScore);
	PlayerBStateRef.SetScore(PlayerBScore);
}

void UJoustRoundCoordinator::SyncPhasePublicState()
{
	AJoustGameState* GameStatePtr = GameState.Get();

	if (GameStatePtr == nullptr || PhaseCoordinator == nullptr)
		return;
	
	GameStatePtr->SetPhaseState(PhaseCoordinator->GetCurrentPhase(), PhaseCoordinator->GetPhaseEndTime());
}

void UJoustRoundCoordinator::SyncStrategyPublicCards()
{
	AJoustGameState* GameStatePtr = GameState.Get();

	if (GameStatePtr == nullptr || StrategyService == nullptr)
		return;
	
	TArray<FName> PublicCardIDs;

	PublicCardIDs.Reserve(StrategyService->GetPublicCards().Num());

	for (UJoustStrategyCardDataAsset* Card : StrategyService->GetPublicCards())
	{
		if (Card != nullptr)
		{
			PublicCardIDs.Add(Card->CardID);
		}
	}

	GameStatePtr->SetPublicStrategyCardIDs(PublicCardIDs);
}

bool UJoustRoundCoordinator::SubmitStrategyBan(bool bPlayerA, const IJoustStrategyInput& StrategyInput)
{
	if (!bRoundActive || FlowState != ERoundFlowState::Strategy || StrategyService == nullptr)
		return false;

	FName CardID;

	if (!StrategyInput.TryGetBannedStrategyCardID(CardID))
		return false;

	if (!StrategyService->SubmitBan(bPlayerA, CardID))
		return false;

	AJoustGameState* GameStatePtr = GameState.Get();
	
	if (GameStatePtr != nullptr)
	{
		GameStatePtr->SetBannedCardIDForPlayer(!bPlayerA, CardID);
	}

	return true;
}

bool UJoustRoundCoordinator::SubmitStrategySelection(bool bPlayerA, const IJoustStrategyInput& StrategyInput)
{
	if (!bRoundActive || FlowState != ERoundFlowState::Strategy || StrategyService == nullptr)
		return false;

	FName CardID;

	if (!StrategyInput.TryGetSelectedStrategyCardID(CardID))
		return false;

	if (!StrategyService->SubmitStrategySelection(bPlayerA, CardID))
		return false;

	if (bPlayerA)
	{
		MarkPlayerAComplete();
	}
	else
	{
		MarkPlayerBComplete();
	}

	return true;
}

bool UJoustRoundCoordinator::SubmitAttack(bool bPlayerA, const IJoustAttackInput& AttackInput)
{
	if (!bRoundActive || FlowState != ERoundFlowState::Attack || AttackService == nullptr || !AttackInput.IsAttackConfirmed())
		return false;

	AJoustPlayerState* PlayerState = bPlayerA ? PlayerAState.Get() : PlayerBState.Get();

	if (PlayerState == nullptr)
		return false;

	FJoustAttackData AttackData{};

	AttackData.AttackPoint = AttackInput.GetAttackPoint();
	AttackData.AttackType = AttackInput.GetAttackType();

	if (!AttackService->SubmitAttack(bPlayerA, AttackData))
		return false;

	PlayerState->SetRemainingAttackUses(AttackData.AttackType, AttackService->GetRemainingUses(bPlayerA, AttackData.AttackType));

	if (bPlayerA)
	{
		MarkPlayerAComplete();
	}
	else
	{
		MarkPlayerBComplete();
	}

	return true;
}

bool UJoustRoundCoordinator::SubmitDefense(bool bPlayerA, const IJoustDefenseInput& DefenseInput)
{
	if (!bRoundActive || FlowState != ERoundFlowState::Defense || bPredictionPlaybackCompleted)
		return false;
	
	FJoustDefenseData& DefenseData = bPlayerA ? PlayerADefenseData : PlayerBDefenseData;

	DefenseData.ShieldPoint = DefenseInput.GetShieldPoint();

	DefenseData.bParryAttempted = DefenseInput.IsParryAttempted();

	DefenseData.ParryInputTime = DefenseData.bParryAttempted ? DefenseInput.GetParryInputTime() : 0.0f;

	return true;
}

bool UJoustRoundCoordinator::SetPlayerStates(AJoustPlayerState* InPlayerAState, AJoustPlayerState* InPlayerBState)
{
	if (bRoundActive)
		return false;

	if (InPlayerAState == nullptr || InPlayerBState == nullptr || InPlayerAState == InPlayerBState)
		return false;
	
	if (AttackService == nullptr || !AttackService->IsMatchUsageInitialized())
		return false;
	
	PlayerAState = InPlayerAState;
	PlayerBState = InPlayerBState;

	if (!SyncAttackUsageStates())
	{
		PlayerAState.Reset();
		PlayerBState.Reset();

		return false;
	}

	return true;
}

void UJoustRoundCoordinator::BeginDestroy()
{
	if (PhaseCoordinator != nullptr)
	{
		PhaseCoordinator->OnPhaseEnded().RemoveAll(this);
	}

	if (AToBPredictionController != nullptr)
	{
		AToBPredictionController->OnPlaybackCompleted().RemoveAll(this);

		AToBPredictionController->StopPlayback();
	}

	if (BToAPredictionController != nullptr)
	{
		BToAPredictionController->OnPlaybackCompleted().RemoveAll(this);

		BToAPredictionController->StopPlayback();
	}

	RandomProvider = nullptr;

	PlayerAState.Reset();
	PlayerBState.Reset();

	GameState.Reset();

	Super::BeginDestroy();
}

bool UJoustRoundCoordinator::BeginTimedPhase(
	EJoustPhase InPhase, 
	float InDurationSeconds, 
	float InShortenRemainingTime, 
	ERoundFlowState InRequiredState, ERoundFlowState InActiveState)
{
	if (!bRoundActive || FlowState != InRequiredState || PhaseCoordinator == nullptr)
		return false;

	if (!PhaseCoordinator->StartTimedPhase(InPhase, InDurationSeconds, InShortenRemainingTime))
		return false;

	FlowState = InActiveState;

	SyncPhasePublicState();

	return true;
}

void UJoustRoundCoordinator::HandlePhaseEnded(EJoustPhase EndedPhase)
{
	if (!bRoundActive)
		return;

	SyncPhasePublicState();

	switch (EndedPhase)
	{

	case EJoustPhase::Strategy:
	{
		if (FlowState != ERoundFlowState::Strategy || StrategyService == nullptr || AttackService == nullptr)
			break;

		AJoustPlayerState* PlayerAStatePtr = PlayerAState.Get();
		AJoustPlayerState* PlayerBStatePtr = PlayerBState.Get();

		if (PlayerAStatePtr == nullptr || PlayerBStatePtr == nullptr)
			break;

		if (!StrategyService->FinalizeStrategy())
			break;

		PlayerAStatePtr->SetCurrentStats(StrategyService->GetPlayerACurrentStats());
		PlayerBStatePtr->SetCurrentStats(StrategyService->GetPlayerBCurrentStats());

		PlayerAStatePtr->SetSelectedStrategyCardID(StrategyService->GetSelectedCardID(true));
		PlayerBStatePtr->SetSelectedStrategyCardID(StrategyService->GetSelectedCardID(false));

		if (!AttackService->PrepareRound(PlayerAStatePtr->GetCurrentStats(), PlayerBStatePtr->GetCurrentStats()))
			break;

		FlowState = ERoundFlowState::ReadyForAttack;

		break;
	}
	
	case EJoustPhase::Attack:
	{
		if (FlowState != ERoundFlowState::Attack || AttackService == nullptr)
			break;

		AttackService->SetSubmissionOpen(false);

		if (!AttackService->AreBothPlayersComplete())
			break;

		AJoustPlayerState* PlayerAStatePtr = PlayerAState.Get();
		AJoustPlayerState* PlayerBStatePtr = PlayerBState.Get();

		if (PlayerAStatePtr == nullptr || PlayerBStatePtr == nullptr)
			break;

		PlayerAAttackData = AttackService->GetPlayerAAttackData();
		PlayerBAttackData = AttackService->GetPlayerBAttackData();

		PlayerADefenseData = FJoustDefenseData{};
		PlayerBDefenseData = FJoustDefenseData{};

		const FJoustPlayerStats& PlayerAStats = PlayerAStatePtr->GetCurrentStats();
		const FJoustPlayerStats& PlayerBStats = PlayerBStatePtr->GetCurrentStats();

		PlayerADefenseData.DefenderId = 0;
		PlayerADefenseData.Stability = PlayerAStats.Stability;
		PlayerADefenseData.Reading = PlayerAStats.Reading;
		PlayerADefenseData.ShieldMobility = PlayerAStats.ShieldMobility;
		PlayerADefenseData.ParrySense = PlayerAStats.ParrySense;

		PlayerBDefenseData.DefenderId = 1;
		PlayerBDefenseData.Stability = PlayerBStats.Stability;
		PlayerBDefenseData.Reading = PlayerBStats.Reading;
		PlayerBDefenseData.ShieldMobility = PlayerBStats.ShieldMobility;
		PlayerBDefenseData.ParrySense = PlayerBStats.ParrySense;

		FlowState = ERoundFlowState::ReadyForDefense;

		if (!PreparePredictions())
			break;

		break;
	}

	case EJoustPhase::Defense:
	{
		if (FlowState == ERoundFlowState::Defense)
		{
			FlowState = ERoundFlowState::ReadyForResolve;
		}
		break;
	}

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

void UJoustRoundCoordinator::HandlePredictionPlaybackCompleted()
{
	if (bPredictionPlaybackCompleted ||
		AToBPredictionController == nullptr ||
		BToAPredictionController == nullptr ||
		!AToBPredictionController->IsCompleted() ||
		!BToAPredictionController->IsCompleted())
		return;

	bPredictionPlaybackCompleted = true;

	PredictionPlaybackCompletedEvent.Broadcast();

	if (!bRoundActive || FlowState != ERoundFlowState::Defense || PhaseCoordinator == nullptr)
		return;
}

bool UJoustRoundCoordinator::SyncAttackUsageStates()
{
	if (AttackService == nullptr)
		return false;

	AJoustPlayerState* PlayerAStatePtr = PlayerAState.Get();

	AJoustPlayerState* PlayerBStatePtr = PlayerBState.Get();

	if (PlayerAStatePtr == nullptr || PlayerBStatePtr == nullptr)
		return false;
	
	TMap<EJoustAttackType, int32> PlayerAUsage;
	TMap<EJoustAttackType, int32> PlayerBUsage;

	if (!AttackService->GetUsageSnapshot(true, PlayerAUsage) || !AttackService->GetUsageSnapshot(false, PlayerBUsage))
		return false;

	PlayerAStatePtr->SetAttackUsageSnapshot(PlayerAUsage);
	PlayerBStatePtr->SetAttackUsageSnapshot(PlayerBUsage);

	return true;
}
