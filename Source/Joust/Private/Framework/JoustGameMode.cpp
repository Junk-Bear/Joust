// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/JoustGameMode.h"
#include "Framework/JoustGameState.h"
#include "Framework/JoustPlayerState.h"
#include "Framework/JoustPlayerController.h"
#include "Framework/JoustPawn.h"
#include "Random/JoustDefaultRandomProvider.h"
#include "Match/JoustPhaseCoordinator.h"
#include "Match/JoustRoundCoordinator.h"
#include "Match/JoustMatchCoordinator.h"
#include "Rules/JoustRuleSetDataAsset.h"
#include "GameFramework/Controller.h"
#include "Framework/JoustAIController.h"
#include "Engine/World.h"
#include "Attack/JoustAttackTypeDataAsset.h"
#include "Prediction/JoustPredictionTypes.h"
#include "Presentation/JoustHUD.h"

AJoustGameMode::AJoustGameMode()
{
	GameStateClass = AJoustGameState::StaticClass();
	PlayerStateClass = AJoustPlayerState::StaticClass();
	PlayerControllerClass = AJoustPlayerController::StaticClass();
	DefaultPawnClass = AJoustPawn::StaticClass();
	HUDClass = AJoustHUD::StaticClass();
}

bool AJoustGameMode::AreParticipantsReady() const
{
	return PlayerAController.IsValid() && PlayerBController.IsValid();
}

bool AJoustGameMode::StartJoustMatch(AController* InRequestingController)
{
	if (!bMatchCoreReady || 
		!IsValid(MatchCoordinator) || 
		!PlayerAController.IsValid() || 
		MatchCoordinator->IsMatchActive() ||
		!IsValid(InRequestingController) ||
		InRequestingController != PlayerAController.Get())
		return false;

	const bool bPlayerBWasEmpty = !PlayerBController.IsValid();

	if (!PrepareMatchParticipants())
		return false;

	if (!MatchCoordinator->StartMatch())
	{
		if (bPlayerBWasEmpty)
		{
			if (AController* PlayerBControllerPtr = PlayerBController.Get())
			{
				PlayerBControllerPtr->Destroy();
			}

			PlayerBController.Reset();
		}

		return false;
	}

	return true;
}

bool AJoustGameMode::SubmitPlayerStrategySelection(AJoustPlayerController* InRequestingController, FName InCardID)
{
	if (!bMatchCoreReady || InCardID.IsNone() || !IsValid(RoundCoordinator) || !IsValid(MatchCoordinator) || !MatchCoordinator->IsMatchActive())
		return false;

	const bool bPlayerA = InRequestingController == PlayerAController.Get();

	if (!bPlayerA && InRequestingController != PlayerBController.Get())
		return false;

	InRequestingController->SetSelectedStrategyCardID(InCardID);

	if (!RoundCoordinator->SubmitStrategySelection(bPlayerA, *InRequestingController))
	{
		InRequestingController->SetSelectedStrategyCardID(NAME_None);

		return false;
	}

	return true;
}

bool AJoustGameMode::SubmitPlayerStrategyBan(AJoustPlayerController* InRequestingController, FName InCardID)
{
	if (
		!bMatchCoreReady ||
		!IsValid(InRequestingController) ||
		InCardID.IsNone() ||
		!IsValid(RoundCoordinator) ||
		!IsValid(MatchCoordinator) ||
		!MatchCoordinator->IsMatchActive())
		return false;

	const bool bPlayerA = InRequestingController == PlayerAController.Get();
	const bool bPlayerB = InRequestingController == PlayerBController.Get();

	if (!bPlayerA && !bPlayerB)
		return false;

	InRequestingController->SetBannedStrategyCardID(InCardID);

	if (RoundCoordinator->SubmitStrategyBan(bPlayerA, *InRequestingController))
		return true;

	InRequestingController->SetBannedStrategyCardID(NAME_None);

	return false;
	
}

void AJoustGameMode::InitGameState()
{
	Super::InitGameState();

	bMatchCoreReady = InitializeMatchCore();
}

void AJoustGameMode::PostLogin(APlayerController* InNewPlayer)
{
	Super::PostLogin(InNewPlayer);

	if (!bMatchCoreReady || !IsValid(InNewPlayer))
		return;

	AJoustPlayerController* JoustPlayerControllerPtr = Cast<AJoustPlayerController>(InNewPlayer);

	if (!IsValid(JoustPlayerControllerPtr))
		return;

	if (IsValid(MatchCoordinator) && MatchCoordinator->IsMatchActive())
	{
		JoustPlayerControllerPtr->StartSpectatingOnly();

		return;
	}

	if (!PlayerAController.IsValid())
	{
		PlayerAController = JoustPlayerControllerPtr;

		return;
	}

	if (!PlayerBController.IsValid())
	{
		PlayerBController = JoustPlayerControllerPtr;

		return;
	}

	JoustPlayerControllerPtr->StartSpectatingOnly();
}

bool AJoustGameMode::InitializeMatchCore()
{
	if (!IsValid(RuleSet) || !IsValid(GetWorld()))
		return false;

	RandomProvider = NewObject<UJoustDefaultRandomProvider>(this);

	PhaseCoordinator = NewObject<UJoustPhaseCoordinator>(this);

	RoundCoordinator = NewObject<UJoustRoundCoordinator>(this);

	MatchCoordinator = NewObject<UJoustMatchCoordinator>(this);

	AJoustGameState* JoustGameStatePtr = GetGameState<AJoustGameState>();

	if (!IsValid(JoustGameStatePtr))
		return false;

	if (!IsValid(RandomProvider) || !IsValid(PhaseCoordinator) || !IsValid(RoundCoordinator) || !IsValid(MatchCoordinator))
		return false;

	JoustGameStatePtr->SetRuleSet(RuleSet.Get());

	RandomProvider->Initialize(InitialRandomSeed);

	PhaseCoordinator->Initialize(GetWorld());

	RoundCoordinator->Initialize(PhaseCoordinator, RuleSet.Get(), *RandomProvider);

	RoundCoordinator->OnPhaseStarted().AddUObject(this, &AJoustGameMode::HandleRoundPhaseStarted);

	RoundCoordinator->OnStrategyBansCompleted().AddUObject(this, &AJoustGameMode::HandleStrategyBansCompleted);

	MatchCoordinator->Initialize(RoundCoordinator, PhaseCoordinator, RuleSet.Get());

	RoundCoordinator->SetGameState(JoustGameStatePtr);

	MatchCoordinator->SetGameState(JoustGameStatePtr);

	return true;
}

bool AJoustGameMode::PrepareMatchParticipants()
{
	if (!bMatchCoreReady || !IsValid(RoundCoordinator) || !IsValid(GetWorld()))
		return false;

	AJoustGameState* GameStatePtr = GetGameState<AJoustGameState>();

	if (!IsValid(GameStatePtr))
		return false;

	AController* PlayerAControllerPtr = PlayerAController.Get();

	if (!IsValid(PlayerAControllerPtr))
		return false;

	AController* PlayerBControllerPtr = PlayerBController.Get();

	AJoustAIController* SpawnedAIControllerPtr = nullptr;

	if (!IsValid(PlayerBControllerPtr))
	{
		FActorSpawnParameters SpawnParameters;

		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		SpawnedAIControllerPtr = GetWorld()->SpawnActor<AJoustAIController>(
			AJoustAIController::StaticClass(),
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			SpawnParameters);

		if (!IsValid(SpawnedAIControllerPtr))
			return false;
		
		PlayerBControllerPtr = SpawnedAIControllerPtr;
	}

	AJoustPlayerState* PlayerAStatePtr = PlayerAControllerPtr->GetPlayerState<AJoustPlayerState>();
	AJoustPlayerState* PlayerBStatePtr = PlayerBControllerPtr->GetPlayerState<AJoustPlayerState>();

	if (!IsValid(PlayerAStatePtr) || !IsValid(PlayerBStatePtr) || PlayerAStatePtr == PlayerBStatePtr)
	{
		if (IsValid(SpawnedAIControllerPtr))
		{
			SpawnedAIControllerPtr->Destroy();
		}

		return false;
	}

	if (!RoundCoordinator->ResetMatchState())
	{
		if (IsValid(SpawnedAIControllerPtr))
		{
			SpawnedAIControllerPtr->Destroy();
		}

		return false;
	}

	PlayerAStatePtr->InitializeStats(PlayerABaseStats);
	PlayerBStatePtr->InitializeStats(PlayerBBaseStats);

	PlayerAStatePtr->ResetMatchState();
	PlayerBStatePtr->ResetMatchState();

	if (!ResetParticipantInput(PlayerAControllerPtr) || !ResetParticipantInput(PlayerBControllerPtr))
	{
		if (IsValid(SpawnedAIControllerPtr))
		{
			SpawnedAIControllerPtr->Destroy();
		}

		return false;
	}

	if (!RoundCoordinator->SetPlayerStates(PlayerAStatePtr, PlayerBStatePtr))
	{
		if (IsValid(SpawnedAIControllerPtr))
		{
			SpawnedAIControllerPtr->Destroy();
		}

		return false;
	}

	GameStatePtr->SetParticipantStates(PlayerAStatePtr, PlayerBStatePtr);

	if (IsValid(SpawnedAIControllerPtr))
	{
		PlayerBController = SpawnedAIControllerPtr;
	}

	return true;
}

bool AJoustGameMode::ResetParticipantInput(AController* InController)
{

	if (AJoustPlayerController* PlayerControllerPtr = Cast<AJoustPlayerController>(InController))
	{
		PlayerControllerPtr->ResetStrategyInput();
		PlayerControllerPtr->ResetAttackInput();
		PlayerControllerPtr->ResetDefenseInput();

		return true;
	}

	if (AJoustAIController* AIControllerPtr = Cast<AJoustAIController>(InController))
	{
		AIControllerPtr->ResetStrategyInput();
		AIControllerPtr->ResetAttackInput();
		AIControllerPtr->ResetDefenseInput();

		return true;
	}

	return false;
}

void AJoustGameMode::HandleRoundPhaseStarted(EJoustPhase InPhase)
{
	if (!IsValid(RoundCoordinator))
		return;

	if (AJoustAIController* PlayerAAIControllerPtr = Cast<AJoustAIController>(PlayerAController.Get()))
	{
		switch (InPhase)
		{
		case EJoustPhase::Strategy:
			SubmitAIStrategy(true, *PlayerAAIControllerPtr);
			break;

		case EJoustPhase::Attack:
			SubmitAIAttack(true, *PlayerAAIControllerPtr);
			break;

		case EJoustPhase::Defense:
			SubmitAIDefense(true, *PlayerAAIControllerPtr);
			break;

		default:
			break;
		}
	}

	if (AJoustAIController* PlayerBAIControllerPtr = Cast<AJoustAIController>(PlayerBController.Get()))
	{
		switch (InPhase)
		{
		case EJoustPhase::Strategy:
			SubmitAIStrategy(false, *PlayerBAIControllerPtr);
			break;

		case EJoustPhase::Attack:
			SubmitAIAttack(false, *PlayerBAIControllerPtr);
			break;

		case EJoustPhase::Defense:
			SubmitAIDefense(false, *PlayerBAIControllerPtr);
			break;

		default:
			break;
		}
	}
}

void AJoustGameMode::HandleStrategyBansCompleted()
{
	if (AJoustAIController* PlayerAAIControllerPtr = Cast<AJoustAIController>(PlayerAController.Get()))
	{
		SubmitAIStrategySelection(true, *PlayerAAIControllerPtr);
	}

	if (AJoustAIController* PlayerBAIControllerPtr = Cast<AJoustAIController>(PlayerBController.Get()))
	{
		SubmitAIStrategySelection(false, *PlayerBAIControllerPtr);
	}
}

bool AJoustGameMode::SubmitAIStrategy(bool bInPlayerA, AJoustAIController & InOutAIController)
{
	if (!IsValid(RoundCoordinator) || !IsValid(RandomProvider))
		return false;
	
	AJoustGameState* JoustGameStatePtr = GetGameState<AJoustGameState>();

	if (!IsValid(JoustGameStatePtr))
		return false;

	InOutAIController.ResetStrategyInput();

	const TArray<FName>& PublicCardsRef = JoustGameStatePtr->GetPublicStrategyCardIDs();

	if (PublicCardsRef.IsEmpty())
		return false;

	InOutAIController.SetBannedStrategyCardID(PublicCardsRef[RandomProvider->GetRandom(0,PublicCardsRef.Num() - 1)]);

	if (RoundCoordinator->SubmitStrategyBan(bInPlayerA, InOutAIController))
		return true;
	
	return SubmitAIStrategySelection(bInPlayerA, InOutAIController);
}

bool AJoustGameMode::SubmitAIStrategySelection(bool bInPlayerA, AJoustAIController& InOutAIController)
{
	if (!IsValid(RoundCoordinator) || !IsValid(RandomProvider))
		return false;
	
	AJoustGameState* JoustGameStatePtr = GetGameState<AJoustGameState>();

	if (!IsValid(JoustGameStatePtr))
		return false;

	const FName BannedCardID = bInPlayerA ? JoustGameStatePtr->GetBannedCardIDForPlayerA() : JoustGameStatePtr->GetBannedCardIDForPlayerB();

	TArray<FName> SelectableCards;

	for (FName Item : JoustGameStatePtr->GetPublicStrategyCardIDs())
	{
		if (!Item.IsNone() && Item != BannedCardID)
		{
			SelectableCards.Add(Item);
		}
	}

	if (SelectableCards.IsEmpty())
		return false;

	InOutAIController.SetSelectedStrategyCardID(SelectableCards[RandomProvider->GetRandom(0,SelectableCards.Num() - 1)]);

	return RoundCoordinator->SubmitStrategySelection(bInPlayerA, InOutAIController);
}

bool AJoustGameMode::SubmitAIAttack(bool bInPlayerA, AJoustAIController& InOutAIController)
{
	if (!IsValid(RoundCoordinator) || !IsValid(RuleSet) || !IsValid(RandomProvider))
		return false;
	
	AJoustPlayerState* PlayerStatePtr = InOutAIController.GetPlayerState<AJoustPlayerState>();

	if (!IsValid(PlayerStatePtr))
		return false;

	TArray<EJoustAttackType> AvailableAttackTypes;

	for (const TPair<EJoustAttackType, TObjectPtr<UJoustAttackTypeDataAsset>>& Item : RuleSet->AttackTypeSettings)
	{
		if (!IsValid(Item.Value))
			continue;

		const int32 RemainingUses = PlayerStatePtr->GetRemainingAttackUses(Item.Key);

		if (RemainingUses == INDEX_NONE || RemainingUses > 0)
		{
			AvailableAttackTypes.Add(Item.Key);
		}
	}

	if (AvailableAttackTypes.IsEmpty())
		return false;

	FVector2D LanceBoxMin = RuleSet->LanceBoxMin;

	FVector2D LanceBoxMax = RuleSet->LanceBoxMax;

	InOutAIController.ResetAttackInput();

	InOutAIController.SetAttackType(AvailableAttackTypes[RandomProvider->GetRandom(0, AvailableAttackTypes.Num() - 1)]);

	InOutAIController.SetAttackPoint(RandomProvider->GetRandom( LanceBoxMin, LanceBoxMax));

	InOutAIController.ConfirmAttack();

	return RoundCoordinator->SubmitAttack(bInPlayerA, InOutAIController);
}

bool AJoustGameMode::SubmitAIDefense(bool bInPlayerA, AJoustAIController& InOutAIController)
{
	if (!IsValid(RoundCoordinator) || !IsValid(RandomProvider))
	{
		return false;
	}

	FJoustPredictionState PredictionState;

	if (!RoundCoordinator->GetDefensePredictionState(bInPlayerA, PredictionState))
		return false;
	
	const int32 CircleIdx = RandomProvider->GetRandom(0, PredictionState.DisplayCircles.Num() - 1);

	InOutAIController.ResetDefenseInput();

	InOutAIController.SetShieldPoint(PredictionState.DisplayCircles[CircleIdx].Center);

	return RoundCoordinator->SubmitDefense(bInPlayerA, InOutAIController);
}
