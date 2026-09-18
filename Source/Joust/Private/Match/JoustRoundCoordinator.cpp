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
	if (IsValid(PhaseCoordinator))
	{
		PhaseCoordinator->OnPhaseEnded().RemoveAll(this);
	}

	if (IsValid(AToBPredictionController))
	{
		AToBPredictionController->OnPredictionStateUpdated().RemoveAll(this);
		AToBPredictionController->OnPlaybackCompleted().RemoveAll(this);
		AToBPredictionController->StopPlayback();
	}

	if (IsValid(BToAPredictionController))
	{
		BToAPredictionController->OnPredictionStateUpdated().RemoveAll(this);
		BToAPredictionController->OnPlaybackCompleted().RemoveAll(this);
		BToAPredictionController->StopPlayback();
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

	if (IsValid(PhaseCoordinator))
	{
		PhaseCoordinator->OnPhaseEnded().AddUObject(this, &UJoustRoundCoordinator::HandlePhaseEnded);
	}

	if (!IsValid(StrategyService))
	{
		StrategyService = NewObject<UJoustStrategyService>(this);
	}

	StrategyService->Initialize(RuleSet, InRandomProvider);

	if (!IsValid(AttackService))
	{
		AttackService = NewObject<UJoustAttackService>(this);
	}

	AttackService->Initialize(RuleSet, InRandomProvider);

	AToBPredictionService = NewObject<UJoustPredictionService>(this);

	BToAPredictionService = NewObject<UJoustPredictionService>(this);

	if (IsValid(AToBPredictionService))
	{
		AToBPredictionService->Initialize(RuleSet, RandomProvider);
	}

	if (IsValid(BToAPredictionService))
	{
		BToAPredictionService->Initialize(RuleSet, RandomProvider);
	}

	AToBPredictionController = NewObject<UJoustPredictionSeriesController>(this);

	BToAPredictionController = NewObject<UJoustPredictionSeriesController>(this);

	if (IsValid(AToBPredictionController))
	{
		AToBPredictionController->Initialize();

		AToBPredictionController->OnPredictionStateUpdated().AddUObject(this, &UJoustRoundCoordinator::SyncPredictionPublicState);

		AToBPredictionController->OnPlaybackCompleted().AddUObject(this, &UJoustRoundCoordinator::HandlePredictionPlaybackCompleted);

	}

	if (IsValid(BToAPredictionController))
	{
		BToAPredictionController->Initialize();

		BToAPredictionController->OnPredictionStateUpdated().AddUObject(this, &UJoustRoundCoordinator::SyncPredictionPublicState);

		BToAPredictionController->OnPlaybackCompleted().AddUObject(this, &UJoustRoundCoordinator::HandlePredictionPlaybackCompleted);
	}

	
}

bool UJoustRoundCoordinator::StartRound(int32 InRoundNumber)
{
	if (!IsValid(PhaseCoordinator) || !IsValid(RuleSet) || RandomProvider == nullptr || !IsValid(StrategyService))
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
	if (!IsValid(PlayerAStatePtr) || !IsValid(PlayerBStatePtr))
		return false;

	const bool bPlayerAHasBanRight = PlayerAStatePtr->HasBanRight();
	const bool bPlayerBHasBanRight = PlayerBStatePtr->HasBanRight();

	PlayerAStatePtr->ResetRoundState();
	PlayerBStatePtr->ResetRoundState();

	if (!StrategyService->PrepareRound(
		PlayerAStatePtr->GetBaseStats(), PlayerBStatePtr->GetBaseStats(),
		PlayerAStatePtr->HasBanRight(), PlayerBStatePtr->HasBanRight()))
		return false;
	
	PlayerAStatePtr->SetHasBanRight(false);
	PlayerBStatePtr->SetHasBanRight(false);

	AJoustGameState* GameStatePtr = GameState.Get();
	if (IsValid(GameStatePtr))
	{
		GameStatePtr->ClearStrategyState();

		const bool bBanSystemEnable = RuleSet->MaxCardBansPerPlayer > 0;

		GameStatePtr->SetPendingStrategyBans(bBanSystemEnable && bPlayerAHasBanRight, bBanSystemEnable && bPlayerBHasBanRight);
	}

	SyncStrategyPublicCards();

	RoundNumber = InRoundNumber;

	ResetRoundData();

	bRoundActive = true;

	FlowState = ERoundFlowState::ReadyForStrategy;

	if (!BeginStrategyPhase())
	{
		StrategyService->EndRound();

		bRoundActive = false;

		FlowState = ERoundFlowState::Finished;

		return false;
	}

	return true;
}

bool UJoustRoundCoordinator::BeginStrategyPhase()
{
	if (!IsValid(RuleSet) || !IsValid(StrategyService) || !StrategyService->IsRoundPrepared())
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
	if (!IsValid(RuleSet) || !IsValid(AttackService) || !AttackService->IsRoundPrepared())
		return false;


	AttackService->SetSubmissionOpen(true);

	const bool bStarted = BeginTimedPhase(
		EJoustPhase::Attack,
		RuleSet->AttackPhaseDuration,
		RuleSet->OnePlayerCompletedRemainingTime,
		ERoundFlowState::ReadyForAttack, ERoundFlowState::Attack
	);

	if (!bStarted)
	{
		AttackService->SetSubmissionOpen(false);
	}

	return bStarted;
}

bool UJoustRoundCoordinator::BeginDefensePhase()
{
	if (!ArePredictionsPrepared() || !IsValid(PhaseCoordinator) || !IsValid(AToBPredictionService) || !IsValid(BToAPredictionService))
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
	if (!bRoundActive || !IsValid(PhaseCoordinator))
		return;

	PhaseCoordinator->MarkPlayerAComplete();

	SyncPhasePublicState();
}

void UJoustRoundCoordinator::MarkPlayerBComplete()
{
	if (!bRoundActive || !IsValid(PhaseCoordinator))
		return;

	PhaseCoordinator->MarkPlayerBComplete();

	SyncPhasePublicState();
}

bool UJoustRoundCoordinator::ResolveRound()
{
	if (!bRoundActive ||
		FlowState != ERoundFlowState::ReadyForResolve ||
		!IsValid(PhaseCoordinator) ||
		!IsValid(RuleSet) ||
		RandomProvider == nullptr)
		return false;

	AJoustPlayerState* PlayerAStatePtr = PlayerAState.Get();
	AJoustPlayerState* PlayerBStatePtr = PlayerBState.Get();

	UJoustMatchCoordinator* MatchCoordinatorPtr = MatchCoordinator.Get();
	if (!IsValid(PlayerAStatePtr) || !IsValid(PlayerBStatePtr) || !IsValid(MatchCoordinatorPtr))
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

	if (IsValid(StrategyService))
	{
		StrategyService->EndRound();
	}

	if (IsValid(AttackService))
	{
		AttackService->EndRound();
	}

	if (IsValid(AToBPredictionController))
	{
		AToBPredictionController->StopPlayback();
	}

	if (IsValid(BToAPredictionController))
	{
		BToAPredictionController->StopPlayback();
	}

	bPredictionPlaybackCompleted = false;

	if (IsValid(AToBPredictionService))
	{
		AToBPredictionService->EndRound();
	}

	if (IsValid(BToAPredictionService))
	{
		BToAPredictionService->EndRound();
	}


	UJoustMatchCoordinator* MatchCoordinatorPtr = MatchCoordinator.Get();
	if (!IsValid(MatchCoordinatorPtr))
		return false;

	return MatchCoordinatorPtr->HandleRoundResolvedCompleted();

}

bool UJoustRoundCoordinator::PreparePredictions()
{
	if (!bRoundActive || FlowState != ERoundFlowState::ReadyForDefense || !IsValid(AToBPredictionService) || !IsValid(BToAPredictionService))
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
	return IsValid(AToBPredictionService) && 
		IsValid(BToAPredictionService) &&
		AToBPredictionService->IsPrepared() && 
		BToAPredictionService->IsPrepared();;
}

bool UJoustRoundCoordinator::StartPredictionPlayback()
{
	if (!bRoundActive || 
		FlowState != ERoundFlowState::ReadyForDefense ||
		!IsValid(AToBPredictionService) || !IsValid(BToAPredictionService) ||
		!IsValid(AToBPredictionController) || !IsValid(BToAPredictionController) ||
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
	AJoustPlayerState& InOutPlayerAState, AJoustPlayerState& InOutPlayerBState, int32 InPlayerAScore, int32 InPlayerBScore)
{
	const FJoustExchangeResult& AToBExchangeRef = CurrentRoundResult.AtoBExchangeResult;
	const FJoustExchangeResult& BToAExchangeRef = CurrentRoundResult.BtoAExchangeResult;

	FJoustAttackHistory PlayerAAttackHistory{};

	PlayerAAttackHistory.RoundNumber = CurrentRoundResult.RoundNumber;
	PlayerAAttackHistory.AttackPoint = AToBExchangeRef.AttackData.AttackPoint;
	PlayerAAttackHistory.AttackType = AToBExchangeRef.AttackData.AttackType;
	PlayerAAttackHistory.OpponentShieldPoint = PlayerBDefenseData.ShieldPoint;
	PlayerAAttackHistory.OpponentDefenseResult = AToBExchangeRef.DefenseResult;
	PlayerAAttackHistory.bScored = AToBExchangeRef.ScoreDelta > 0;
	PlayerAAttackHistory.bOpponentUnhorsed = AToBExchangeRef.bDefenderUnhorsed;

	FJoustDefenseHistory PlayerADefenseHistory{};

	PlayerADefenseHistory.RoundNumber = CurrentRoundResult.RoundNumber;
	PlayerADefenseHistory.OpponentAttackPoint = BToAExchangeRef.AttackData.AttackPoint;
	PlayerADefenseHistory.OpponentAttackType = BToAExchangeRef.AttackData.AttackType;
	PlayerADefenseHistory.ShieldPoint = PlayerADefenseData.ShieldPoint;
	PlayerADefenseHistory.DefenseResult = BToAExchangeRef.DefenseResult;
	PlayerADefenseHistory.bConcededScore = BToAExchangeRef.ScoreDelta > 0;
	PlayerADefenseHistory.bUnhorsed = BToAExchangeRef.bDefenderUnhorsed;

	FJoustAttackHistory PlayerBAttackHistory{};

	PlayerBAttackHistory.RoundNumber = CurrentRoundResult.RoundNumber;
	PlayerBAttackHistory.AttackPoint = BToAExchangeRef.AttackData.AttackPoint;
	PlayerBAttackHistory.AttackType = BToAExchangeRef.AttackData.AttackType;
	PlayerBAttackHistory.OpponentShieldPoint = PlayerADefenseData.ShieldPoint;
	PlayerBAttackHistory.OpponentDefenseResult = BToAExchangeRef.DefenseResult;
	PlayerBAttackHistory.bScored = BToAExchangeRef.ScoreDelta > 0;
	PlayerBAttackHistory.bOpponentUnhorsed = BToAExchangeRef.bDefenderUnhorsed;

	FJoustDefenseHistory PlayerBDefenseHistory{};

	PlayerBDefenseHistory.RoundNumber = CurrentRoundResult.RoundNumber;
	PlayerBDefenseHistory.OpponentAttackPoint = AToBExchangeRef.AttackData.AttackPoint;
	PlayerBDefenseHistory.OpponentAttackType = AToBExchangeRef.AttackData.AttackType;
	PlayerBDefenseHistory.ShieldPoint = PlayerBDefenseData.ShieldPoint;
	PlayerBDefenseHistory.DefenseResult = AToBExchangeRef.DefenseResult;
	PlayerBDefenseHistory.bConcededScore = AToBExchangeRef.ScoreDelta > 0;
	PlayerBDefenseHistory.bUnhorsed = AToBExchangeRef.bDefenderUnhorsed;

	InOutPlayerAState.AddAttackHistory(PlayerAAttackHistory);
	InOutPlayerAState.AddDefenseHistory(PlayerADefenseHistory);

	InOutPlayerBState.AddAttackHistory(PlayerBAttackHistory);
	InOutPlayerBState.AddDefenseHistory(PlayerBDefenseHistory);

	InOutPlayerAState.SetLastDefenseResult(BToAExchangeRef.DefenseResult);

	InOutPlayerBState.SetLastDefenseResult(AToBExchangeRef.DefenseResult);

	if (BToAExchangeRef.DefenseResult.bIsEdgeParry)
	{
		InOutPlayerAState.SetHasBanRight(true);
	}

	if (AToBExchangeRef.DefenseResult.bIsEdgeParry)
	{
		InOutPlayerBState.SetHasBanRight(true);
	}

	if (BToAExchangeRef.bDefenderUnhorsed)
	{
		InOutPlayerAState.SetUnhorsed(true);
	}

	if (AToBExchangeRef.bDefenderUnhorsed)
	{
		InOutPlayerBState.SetUnhorsed(true);
	}

	InOutPlayerAState.SetScore(InPlayerAScore);
	InOutPlayerBState.SetScore(InPlayerBScore);
}

void UJoustRoundCoordinator::SyncPhasePublicState()
{
	AJoustGameState* GameStatePtr = GameState.Get();
	if (!IsValid(GameStatePtr) || !IsValid(PhaseCoordinator))
		return;
	
	GameStatePtr->SetPhaseState(PhaseCoordinator->GetCurrentPhase(), PhaseCoordinator->GetPhaseEndTime());
}

void UJoustRoundCoordinator::SyncStrategyPublicCards()
{
	AJoustGameState* GameStatePtr = GameState.Get();
	if (!IsValid(GameStatePtr) || !IsValid(StrategyService))
		return;
	
	TArray<FName> PublicCardIDs;

	PublicCardIDs.Reserve(StrategyService->GetPublicCards().Num());

	for (UJoustStrategyCardDataAsset* Item : StrategyService->GetPublicCards())
	{
		if (IsValid(Item))
		{
			PublicCardIDs.Add(Item->CardID);
		}
	}

	GameStatePtr->SetPublicStrategyCardIDs(PublicCardIDs);
}

bool UJoustRoundCoordinator::SubmitDefaultStrategySelection(bool bInPlayerA)
{
	if (!IsValid(StrategyService))
		return false;

	if (StrategyService->IsPlayerComplete(bInPlayerA))
		return true;

	TArray<UJoustStrategyCardDataAsset*> SelectableCards;

	if (!StrategyService->GetSelectableCards(bInPlayerA, SelectableCards))
		return false;

	for (UJoustStrategyCardDataAsset* Item : SelectableCards)
	{
		if (IsValid(Item) && StrategyService->SubmitStrategySelection(bInPlayerA, Item->CardID))
			return true;
	}

	return false;
}

bool UJoustRoundCoordinator::SubmitDefaultAttack(bool bInPlayerA)
{
	if (!IsValid(AttackService) || !IsValid(RuleSet))
		return false;

	if (AttackService->IsPlayerComplete(bInPlayerA))
		return true;

	AJoustPlayerState* PlayerStatePtr = bInPlayerA ? PlayerAState.Get() : PlayerBState.Get();
	if (!IsValid(PlayerStatePtr))
		return false;

	static constexpr EJoustAttackType DefaultAttackTypeOrder[] = { EJoustAttackType::Normal, EJoustAttackType::Strong, EJoustAttackType::Trick, EJoustAttackType::Slow};

	const FVector2D DefaultAttackPoint = (RuleSet->LanceBoxMin + RuleSet->LanceBoxMax) * 0.5f;

	for (EJoustAttackType Item : DefaultAttackTypeOrder)
	{
		if (!AttackService->CanPlayerUseAttackType(bInPlayerA, Item))
			continue;

		FJoustAttackData AttackData{};

		AttackData.AttackPoint = DefaultAttackPoint;
		AttackData.AttackType = Item;

		if (!AttackService->SubmitAttack(bInPlayerA, AttackData))
			continue;

		PlayerStatePtr->SetRemainingAttackUses(
			Item,
			AttackService->GetRemainingUses(bInPlayerA, Item));

		return true;
	}

	return false;
}

void UJoustRoundCoordinator::SyncPredictionPublicState()
{
	AJoustGameState* GameStatePtr = GameState.Get();
	if (!IsValid(GameStatePtr))
		return;

	FJoustPredictionState PlayerAPredictionState;
	FJoustPredictionState PlayerBPredictionState;

	if (IsValid(BToAPredictionController))
	{
		BToAPredictionController->BuildPredictionState(PlayerAPredictionState);
	}

	if (IsValid(AToBPredictionController))
	{
		AToBPredictionController->BuildPredictionState(PlayerBPredictionState);
	}

	GameStatePtr->SetPredictionStates(PlayerAPredictionState, PlayerBPredictionState);
}

bool UJoustRoundCoordinator::SubmitStrategyBan(bool bInPlayerA, const IJoustStrategyInput& InStrategyInput)
{
	if (!bRoundActive || FlowState != ERoundFlowState::Strategy || !IsValid(StrategyService))
		return false;

	FName CardID;

	if (!InStrategyInput.TryGetBannedStrategyCardID(CardID))
		return false;

	if (!StrategyService->SubmitBan(bInPlayerA, CardID))
		return false;

	AJoustGameState* GameStatePtr = GameState.Get();
	if (IsValid(GameStatePtr))
	{
		GameStatePtr->ApplyStrategyBan(!bInPlayerA, CardID);
	}

	if (StrategyService->AreBansComplete())
	{
		StrategyBansCompletedEvent.Broadcast();
	}

	return true;
}

bool UJoustRoundCoordinator::SubmitStrategySelection(bool bInPlayerA, const IJoustStrategyInput& InStrategyInput)
{
	if (!bRoundActive || FlowState != ERoundFlowState::Strategy || !IsValid(StrategyService))
		return false;

	FName CardID;

	if (!InStrategyInput.TryGetSelectedStrategyCardID(CardID))
		return false;

	if (!StrategyService->SubmitStrategySelection(bInPlayerA, CardID))
		return false;

	if (bInPlayerA)
	{
		MarkPlayerAComplete();
	}
	else
	{
		MarkPlayerBComplete();
	}

	return true;
}

bool UJoustRoundCoordinator::SubmitAttack(bool bInPlayerA, const IJoustAttackInput& InAttackInput)
{
	if (!bRoundActive || FlowState != ERoundFlowState::Attack || !IsValid(AttackService) || !InAttackInput.IsAttackConfirmed())
		return false;

	AJoustPlayerState* PlayerStatePtr = bInPlayerA ? PlayerAState.Get() : PlayerBState.Get();
	if (!IsValid(PlayerStatePtr))
		return false;

	FJoustAttackData AttackData{};

	AttackData.AttackPoint = InAttackInput.GetAttackPoint();
	AttackData.AttackType = InAttackInput.GetAttackType();

	if (!AttackService->SubmitAttack(bInPlayerA, AttackData))
		return false;

	PlayerStatePtr->SetRemainingAttackUses(AttackData.AttackType, AttackService->GetRemainingUses(bInPlayerA, AttackData.AttackType));

	if (bInPlayerA)
	{
		MarkPlayerAComplete();
	}
	else
	{
		MarkPlayerBComplete();
	}

	return true;
}

bool UJoustRoundCoordinator::SubmitDefense(bool bInPlayerA, const IJoustDefenseInput& InDefenseInput)
{
	if (!bRoundActive || FlowState != ERoundFlowState::Defense || bPredictionPlaybackCompleted)
		return false;
	
	FJoustDefenseData& DefenseDataRef = bInPlayerA ? PlayerADefenseData : PlayerBDefenseData;

	DefenseDataRef.ShieldPoint = InDefenseInput.GetShieldPoint();

	DefenseDataRef.bParryAttempted = InDefenseInput.IsParryAttempted();

	DefenseDataRef.ParryInputTime = DefenseDataRef.bParryAttempted ? InDefenseInput.GetParryInputTime() : 0.0f;

	return true;
}

bool UJoustRoundCoordinator::SetPlayerStates(AJoustPlayerState* InPlayerAState, AJoustPlayerState* InPlayerBState)
{
	if (bRoundActive)
		return false;

	if (!IsValid(InPlayerAState) || !IsValid(InPlayerBState) || InPlayerAState == InPlayerBState)
		return false;
	
	if (!IsValid(AttackService) || !AttackService->IsMatchUsageInitialized())
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

bool UJoustRoundCoordinator::ResetMatchState()
{
	if (bRoundActive ||
		!IsValid(PhaseCoordinator) ||
		PhaseCoordinator->IsPhaseActive() ||
		!IsValid(StrategyService) ||
		!IsValid(AttackService))
		return false;
	
	if (FlowState != ERoundFlowState::Idle && FlowState != ERoundFlowState::Finished)
		return false;
	
	StrategyService->EndRound();

	if (IsValid(AToBPredictionController))
	{
		AToBPredictionController->StopPlayback();
	}

	if (IsValid(BToAPredictionController))
	{
		BToAPredictionController->StopPlayback();
	}

	if (IsValid(AToBPredictionService))
	{
		AToBPredictionService->EndRound();
	}

	if (IsValid(BToAPredictionService))
	{
		BToAPredictionService->EndRound();
	}

	if (!AttackService->ResetMatchUsage())
		return false;
	
	PlayerAState.Reset();
	PlayerBState.Reset();

	RoundNumber = 0;

	bPredictionPlaybackCompleted = false;

	ResetRoundData();

	FlowState = ERoundFlowState::Idle;

	return true;
}

bool UJoustRoundCoordinator::GetDefensePredictionState(bool bInPlayerA, FJoustPredictionState& OutState) const
{
	OutState = FJoustPredictionState{};

	const UJoustPredictionSeriesController* PredictionControllerPtr = bInPlayerA ? BToAPredictionController.Get() : AToBPredictionController.Get();
	if (!IsValid(PredictionControllerPtr))
		return false;

	PredictionControllerPtr->BuildPredictionState(OutState);

	return (OutState.bIsPredictionVisible && !OutState.DisplayCircles.IsEmpty()) || OutState.bIsAttackPointRevealed;
}

void UJoustRoundCoordinator::SetMatchCoordinator(UJoustMatchCoordinator* InMatchCoordinator)
{
	MatchCoordinator = InMatchCoordinator;
}

void UJoustRoundCoordinator::SetGameState(AJoustGameState* InGameState)
{
	GameState = InGameState;

	SyncPredictionPublicState();
}

void UJoustRoundCoordinator::BeginDestroy()
{
	if (IsValid(PhaseCoordinator))
	{
		PhaseCoordinator->OnPhaseEnded().RemoveAll(this);
	}

	if (IsValid(AToBPredictionController))
	{
		AToBPredictionController->OnPredictionStateUpdated().RemoveAll(this);
		AToBPredictionController->OnPlaybackCompleted().RemoveAll(this);

		AToBPredictionController->StopPlayback();
	}

	if (IsValid(BToAPredictionController))
	{
		BToAPredictionController->OnPredictionStateUpdated().RemoveAll(this);
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
	if (!bRoundActive || FlowState != InRequiredState || !IsValid(PhaseCoordinator))
		return false;

	if (!PhaseCoordinator->StartTimedPhase(InPhase, InDurationSeconds, InShortenRemainingTime))
		return false;

	FlowState = InActiveState;

	SyncPhasePublicState();

	PhaseStartedEvent.Broadcast(InPhase);

	return true;
}

void UJoustRoundCoordinator::HandlePhaseEnded(EJoustPhase InEndedPhase)
{
	if (!bRoundActive)
		return;

	SyncPhasePublicState();

	switch (InEndedPhase)
	{

	case EJoustPhase::Strategy:
	{
		if (FlowState != ERoundFlowState::Strategy || !IsValid(StrategyService) || !IsValid(AttackService))
			break;

		AJoustPlayerState* PlayerAStatePtr = PlayerAState.Get();
		AJoustPlayerState* PlayerBStatePtr = PlayerBState.Get();
		if (!IsValid(PlayerAStatePtr) || !IsValid(PlayerBStatePtr))
			break;

		if (!StrategyService->AreBansComplete())
		{
			StrategyService->SkipPendingBan(true);
			StrategyService->SkipPendingBan(false);

			if (AJoustGameState* GameStatePtr = GameState.Get())
				GameStatePtr->SetPendingStrategyBans(false, false);
		}

		if (!SubmitDefaultStrategySelection(true) ||!SubmitDefaultStrategySelection(false))
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

		BeginAttackPhase();

		break;
	}
	
	case EJoustPhase::Attack:
	{
		if (FlowState != ERoundFlowState::Attack || !IsValid(AttackService))
			break;

		if (!SubmitDefaultAttack(true) || !SubmitDefaultAttack(false))
		{
			AttackService->SetSubmissionOpen(false);

			break;
		}

		AttackService->SetSubmissionOpen(false);

		if (!AttackService->AreBothPlayersComplete())
			break;

		AJoustPlayerState* PlayerAStatePtr = PlayerAState.Get();
		AJoustPlayerState* PlayerBStatePtr = PlayerBState.Get();
		if (!IsValid(PlayerAStatePtr) || !IsValid(PlayerBStatePtr))
			break;

		PlayerAAttackData = AttackService->GetPlayerAAttackData();
		PlayerBAttackData = AttackService->GetPlayerBAttackData();

		PlayerADefenseData = FJoustDefenseData{};
		PlayerBDefenseData = FJoustDefenseData{};

		const FJoustPlayerStats& PlayerAStatsRef = PlayerAStatePtr->GetCurrentStats();
		const FJoustPlayerStats& PlayerBStatsRef = PlayerBStatePtr->GetCurrentStats();

		PlayerADefenseData.DefenderId = 0;
		PlayerADefenseData.Stability = PlayerAStatsRef.Stability;
		PlayerADefenseData.Reading = PlayerAStatsRef.Reading;
		PlayerADefenseData.ShieldMobility = PlayerAStatsRef.ShieldMobility;
		PlayerADefenseData.ParrySense = PlayerAStatsRef.ParrySense;

		PlayerBDefenseData.DefenderId = 1;
		PlayerBDefenseData.Stability = PlayerBStatsRef.Stability;
		PlayerBDefenseData.Reading = PlayerBStatsRef.Reading;
		PlayerBDefenseData.ShieldMobility = PlayerBStatsRef.ShieldMobility;
		PlayerBDefenseData.ParrySense = PlayerBStatsRef.ParrySense;

		FlowState = ERoundFlowState::ReadyForDefense;

		if (!PreparePredictions())
			break;

		BeginDefensePhase();

		break;
	}

	case EJoustPhase::Defense:
	{
		if (FlowState != ERoundFlowState::Defense)
			break;
		
		FlowState = ERoundFlowState::ReadyForResolve;

		ResolveRound();

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

	AJoustGameState* GameStatePtr = GameState.Get();
	if (IsValid(GameStatePtr))
	{
		GameStatePtr->ClearPredictionStates();
	}
}

void UJoustRoundCoordinator::HandlePredictionPlaybackCompleted()
{
	if (bPredictionPlaybackCompleted ||
		!IsValid(AToBPredictionController) ||
		!IsValid(BToAPredictionController) ||
		!AToBPredictionController->IsCompleted() ||
		!BToAPredictionController->IsCompleted())
		return;

	bPredictionPlaybackCompleted = true;

	PredictionPlaybackCompletedEvent.Broadcast();
}

bool UJoustRoundCoordinator::SyncAttackUsageStates()
{
	if (!IsValid(AttackService))
		return false;

	AJoustPlayerState* PlayerAStatePtr = PlayerAState.Get();

	AJoustPlayerState* PlayerBStatePtr = PlayerBState.Get();
	if (!IsValid(PlayerAStatePtr) || !IsValid(PlayerBStatePtr))
		return false;
	
	TMap<EJoustAttackType, int32> PlayerAUsage;
	TMap<EJoustAttackType, int32> PlayerBUsage;

	if (!AttackService->GetUsageSnapshot(true, PlayerAUsage) || !AttackService->GetUsageSnapshot(false, PlayerBUsage))
		return false;

	PlayerAStatePtr->SetAttackUsageSnapshot(PlayerAUsage);
	PlayerBStatePtr->SetAttackUsageSnapshot(PlayerBUsage);

	return true;
}
