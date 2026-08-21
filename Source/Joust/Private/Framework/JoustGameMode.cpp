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

AJoustGameMode::AJoustGameMode()
{
	GameStateClass = AJoustGameState::StaticClass();
	PlayerStateClass = AJoustPlayerState::StaticClass();
	PlayerControllerClass = AJoustPlayerController::StaticClass();
	DefaultPawnClass = AJoustPawn::StaticClass();
}

bool AJoustGameMode::AreParticipantsReady() const
{
	return PlayerAController.IsValid() && PlayerBController.IsValid();
}

bool AJoustGameMode::StartJoustMatch()
{
	if (!bMatchCoreReady || MatchCoordinator == nullptr || !PlayerAController.IsValid() || MatchCoordinator->IsMatchActive())
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

void AJoustGameMode::InitGameState()
{
	Super::InitGameState();

	bMatchCoreReady = InitializeMatchCore();
}

void AJoustGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (!bMatchCoreReady || NewPlayer == nullptr)
		return;

	AJoustPlayerController* JoustPlayerController = Cast<AJoustPlayerController>(NewPlayer);

	if (JoustPlayerController == nullptr)
		return;

	if (MatchCoordinator != nullptr && MatchCoordinator->IsMatchActive())
	{
		JoustPlayerController->StartSpectatingOnly();

		return;
	}

	if (!PlayerAController.IsValid())
	{
		PlayerAController = JoustPlayerController;

		return;
	}

	if (!PlayerBController.IsValid())
	{
		PlayerBController = JoustPlayerController;

		return;
	}

	JoustPlayerController->StartSpectatingOnly();
}

bool AJoustGameMode::InitializeMatchCore()
{
	if (RuleSet == nullptr || GetWorld() == nullptr)
		return false;

	RandomProvider = NewObject<UJoustDefaultRandomProvider>(this);

	PhaseCoordinator = NewObject<UJoustPhaseCoordinator>(this);

	RoundCoordinator = NewObject<UJoustRoundCoordinator>(this);

	MatchCoordinator = NewObject<UJoustMatchCoordinator>(this);

	AJoustGameState* JoustGameState = GetGameState<AJoustGameState>();

	if (JoustGameState == nullptr)
		return false;

	if (RandomProvider == nullptr || PhaseCoordinator == nullptr || RoundCoordinator == nullptr || MatchCoordinator == nullptr)
		return false;

	RandomProvider->Initialize(InitialRandomSeed);

	PhaseCoordinator->Initialize(GetWorld());

	RoundCoordinator->Initialize(PhaseCoordinator, RuleSet.Get(), *RandomProvider);

	RoundCoordinator->OnPhaseStarted().AddUObject(this, &AJoustGameMode::HandleRoundPhaseStarted);

	RoundCoordinator->OnStrategyBansCompleted().AddUObject(this, &AJoustGameMode::HandleStrategyBansCompleted);

	MatchCoordinator->Initialize(RoundCoordinator, PhaseCoordinator, RuleSet.Get());

	RoundCoordinator->SetGameState(JoustGameState);

	MatchCoordinator->SetGameState(JoustGameState);

	return true;
}

bool AJoustGameMode::PrepareMatchParticipants()
{
	if (!bMatchCoreReady || RoundCoordinator == nullptr || GetWorld() == nullptr)
		return false;

	AController* PlayerAControllerPtr = PlayerAController.Get();

	if (PlayerAControllerPtr == nullptr)
		return false;

	AController* PlayerBControllerPtr = PlayerBController.Get();

	AJoustAIController* SpawnedAIController = nullptr;

	if (PlayerBControllerPtr == nullptr)
	{
		FActorSpawnParameters SpawnParameters;

		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		SpawnedAIController = GetWorld()->SpawnActor<AJoustAIController>(
			AJoustAIController::StaticClass(),
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			SpawnParameters);

		if (SpawnedAIController == nullptr)
			return false;
		
		PlayerBControllerPtr = SpawnedAIController;
	}

	AJoustPlayerState* PlayerAState = PlayerAControllerPtr->GetPlayerState<AJoustPlayerState>();
	AJoustPlayerState* PlayerBState = PlayerBControllerPtr->GetPlayerState<AJoustPlayerState>();

	if (PlayerAState == nullptr || PlayerBState == nullptr || PlayerAState == PlayerBState)
	{
		if (SpawnedAIController != nullptr)
		{
			SpawnedAIController->Destroy();
		}

		return false;
	}

	if (!RoundCoordinator->ResetMatchState())
	{
		if (SpawnedAIController != nullptr)
		{
			SpawnedAIController->Destroy();
		}

		return false;
	}

	PlayerAState->InitializeStats(PlayerABaseStats);
	PlayerBState->InitializeStats(PlayerBBaseStats);

	PlayerAState->ResetMatchState();
	PlayerBState->ResetMatchState();

	if (!ResetParticipantInput(PlayerAControllerPtr) || !ResetParticipantInput(PlayerBControllerPtr))
	{
		if (SpawnedAIController != nullptr)
		{
			SpawnedAIController->Destroy();
		}

		return false;
	}

	if (!RoundCoordinator->SetPlayerStates(PlayerAState, PlayerBState))
	{
		if (SpawnedAIController != nullptr)
		{
			SpawnedAIController->Destroy();
		}

		return false;
	}

	if (SpawnedAIController != nullptr)
	{
		PlayerBController = SpawnedAIController;
	}

	return true;
}

bool AJoustGameMode::ResetParticipantInput(AController* InController)
{

	if (AJoustPlayerController* PlayerController = Cast<AJoustPlayerController>(InController))
	{
		PlayerController->ResetStrategyInput();
		PlayerController->ResetAttackInput();
		PlayerController->ResetDefenseInput();

		return true;
	}

	if (AJoustAIController* AIController = Cast<AJoustAIController>(InController))
	{
		AIController->ResetStrategyInput();
		AIController->ResetAttackInput();
		AIController->ResetDefenseInput();

		return true;
	}

	return false;
}

void AJoustGameMode::HandleRoundPhaseStarted(EJoustPhase InPhase)
{
	if (RoundCoordinator == nullptr)
		return;

	if (AJoustAIController* PlayerAAI = Cast<AJoustAIController>(PlayerAController.Get()))
	{
		switch (InPhase)
		{
		case EJoustPhase::Strategy:
			SubmitAIStrategy(true, *PlayerAAI);
			break;

		case EJoustPhase::Attack:
			SubmitAIAttack(true, *PlayerAAI);
			break;

		case EJoustPhase::Defense:
			SubmitAIDefense(true, *PlayerAAI);
			break;

		default:
			break;
		}
	}

	if (AJoustAIController* PlayerBAI = Cast<AJoustAIController>(PlayerBController.Get()))
	{
		switch (InPhase)
		{
		case EJoustPhase::Strategy:
			SubmitAIStrategy(false, *PlayerBAI);
			break;

		case EJoustPhase::Attack:
			SubmitAIAttack(false, *PlayerBAI);
			break;

		case EJoustPhase::Defense:
			SubmitAIDefense(false, *PlayerBAI);
			break;

		default:
			break;
		}
	}
}

void AJoustGameMode::HandleStrategyBansCompleted()
{
	if (AJoustAIController* PlayerAAI = Cast<AJoustAIController>(PlayerAController.Get()))
	{
		SubmitAIStrategySelection(true, *PlayerAAI);
	}

	if (AJoustAIController* PlayerBAI = Cast<AJoustAIController>(PlayerBController.Get()))
	{
		SubmitAIStrategySelection(false, *PlayerBAI);
	}
}

bool AJoustGameMode::SubmitAIStrategy(bool bPlayerA, AJoustAIController & AIController)
{
	if (RoundCoordinator == nullptr || RandomProvider == nullptr)
		return false;
	
	AJoustGameState* JoustGameState = GetGameState<AJoustGameState>();

	if (JoustGameState == nullptr)
		return false;

	AIController.ResetStrategyInput();

	const TArray<FName>& PublicCards = JoustGameState->GetPublicStrategyCardIDs();

	if (PublicCards.IsEmpty())
		return false;

	AIController.SetBannedStrategyCardID(PublicCards[RandomProvider->GetRandom(0,PublicCards.Num() - 1)]);

	if (RoundCoordinator->SubmitStrategyBan(bPlayerA, AIController))
		return true;
	
	return SubmitAIStrategySelection(bPlayerA, AIController);
}

bool AJoustGameMode::SubmitAIStrategySelection(bool bPlayerA, AJoustAIController& AIController)
{
	if (RoundCoordinator == nullptr || RandomProvider == nullptr)
		return false;
	
	AJoustGameState* JoustGameState = GetGameState<AJoustGameState>();

	if (JoustGameState == nullptr)
		return false;

	const FName BannedCardID = bPlayerA ? JoustGameState->GetBannedCardIDForPlayerA() : JoustGameState->GetBannedCardIDForPlayerB();

	TArray<FName> SelectableCards;

	for (FName CardID : JoustGameState->GetPublicStrategyCardIDs())
	{
		if (!CardID.IsNone() && CardID != BannedCardID)
		{
			SelectableCards.Add(CardID);
		}
	}

	if (SelectableCards.IsEmpty())
		return false;

	AIController.SetSelectedStrategyCardID(SelectableCards[RandomProvider->GetRandom(0,SelectableCards.Num() - 1)]);

	return RoundCoordinator->SubmitStrategySelection(bPlayerA, AIController);
}

bool AJoustGameMode::SubmitAIAttack(bool bPlayerA, AJoustAIController& AIController)
{
	if (RoundCoordinator == nullptr || RuleSet == nullptr || RandomProvider == nullptr)
		return false;
	
	AJoustPlayerState* PlayerState = AIController.GetPlayerState<AJoustPlayerState>();

	if (PlayerState == nullptr)
		return false;

	TArray<EJoustAttackType> AvailableAttackTypes;

	for (const TPair<EJoustAttackType, TObjectPtr<UJoustAttackTypeDataAsset>>& PairItem : RuleSet->AttackTypeSettings)
	{
		if (PairItem.Value == nullptr)
			continue;

		const int32 RemainingUses = PlayerState->GetRemainingAttackUses(PairItem.Key);

		if (RemainingUses == INDEX_NONE || RemainingUses > 0)
		{
			AvailableAttackTypes.Add(PairItem.Key);
		}
	}

	if (AvailableAttackTypes.IsEmpty())
		return false;

	FVector2D LanceBoxMin = RuleSet->LanceBoxMin;

	FVector2D LanceBoxMax = RuleSet->LanceBoxMax;

	AIController.ResetAttackInput();

	AIController.SetAttackType(AvailableAttackTypes[RandomProvider->GetRandom(0, AvailableAttackTypes.Num() - 1)]);

	AIController.SetAttackPoint(RandomProvider->GetRandom( LanceBoxMin, LanceBoxMax));

	AIController.ConfirmAttack();

	return RoundCoordinator->SubmitAttack(bPlayerA, AIController);
}

bool AJoustGameMode::SubmitAIDefense(bool bPlayerA, AJoustAIController& AIController)
{
	if (RoundCoordinator == nullptr || RandomProvider == nullptr)
	{
		return false;
	}

	FJoustPredictionState PredictionState;

	if (!RoundCoordinator->GetDefensePredictionState(bPlayerA, PredictionState))
		return false;
	
	const int32 CircleIdx = RandomProvider->GetRandom(0, PredictionState.DisplayCircles.Num() - 1);

	AIController.ResetDefenseInput();

	AIController.SetShieldPoint(PredictionState.DisplayCircles[CircleIdx].Center);

	return RoundCoordinator->SubmitDefense(bPlayerA, AIController);
}
