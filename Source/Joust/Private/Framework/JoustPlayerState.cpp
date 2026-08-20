// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/JoustPlayerState.h"
#include "Net/UnrealNetwork.h"

void AJoustPlayerState::InitializeStats(
	const FJoustPlayerStats& InBaseStats)
{
	BaseStats = InBaseStats;
	CurrentStats = InBaseStats;
}

void AJoustPlayerState::ResetRoundState()
{
	CurrentStats = BaseStats;

	SelectedStrategyCardID = NAME_None;

	LastDefenseResult = FJoustDefenseResult{};
}

void AJoustPlayerState::ResetMatchState()
{
	ResetRoundState();

	SetScore(0.0f);

	RemainingAttackUses.Reset();
	ReplicatedAttackUsageStates.Reset();

	AttackHistory.Reset();
	DefenseHistory.Reset();

	bHasBanRight = false;
	bUnhorsed = false;
}

void AJoustPlayerState::SetRemainingAttackUses(EJoustAttackType AttackType, int32 RemainingUses)
{
	RemainingAttackUses.Add(AttackType, RemainingUses);

	for (FJoustAttackUsageState& Item : ReplicatedAttackUsageStates)
	{
		if (Item.AttackType == AttackType)
		{
			Item.RemainingUses = RemainingUses;

			return;
		}
	}

	FJoustAttackUsageState& NewUsageState = ReplicatedAttackUsageStates.AddDefaulted_GetRef();
	NewUsageState.AttackType = AttackType;
	NewUsageState.RemainingUses = RemainingUses;
}

int32 AJoustPlayerState::GetRemainingAttackUses(EJoustAttackType AttackType) const
{
	const int32* RemainingUses = RemainingAttackUses.Find(AttackType);

	return RemainingUses != nullptr ? *RemainingUses : 0;
}

void AJoustPlayerState::AddAttackHistory(const FJoustAttackHistory& InAttackHistory)
{
	AttackHistory.Add(InAttackHistory);
}

void AJoustPlayerState::AddDefenseHistory(const FJoustDefenseHistory& InDefenseHistory)
{
	DefenseHistory.Add(InDefenseHistory);
}

void AJoustPlayerState::SetAttackUsageSnapshot(const TMap<EJoustAttackType, int32>& InRemainingAttackUses)
{
	RemainingAttackUses.Reset();
	ReplicatedAttackUsageStates.Reset();

	RemainingAttackUses.Reserve(InRemainingAttackUses.Num());
	ReplicatedAttackUsageStates.Reserve(InRemainingAttackUses.Num());

	for (const TPair<EJoustAttackType, int32>& PairItem : InRemainingAttackUses)
	{
		SetRemainingAttackUses(PairItem.Key,PairItem.Value);
	}

	ReplicatedAttackUsageStates.Sort([](const FJoustAttackUsageState& A,const FJoustAttackUsageState& B)
		{
			return static_cast<uint8>(A.AttackType) < static_cast<uint8>(B.AttackType);
		});
}

void AJoustPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AJoustPlayerState, BaseStats);

	DOREPLIFETIME(AJoustPlayerState, CurrentStats);

	DOREPLIFETIME(AJoustPlayerState, SelectedStrategyCardID);

	DOREPLIFETIME(AJoustPlayerState, ReplicatedAttackUsageStates);

	DOREPLIFETIME(AJoustPlayerState, AttackHistory);

	DOREPLIFETIME(AJoustPlayerState, DefenseHistory);

	DOREPLIFETIME(AJoustPlayerState, LastDefenseResult);

	DOREPLIFETIME(AJoustPlayerState, bHasBanRight);

	DOREPLIFETIME(AJoustPlayerState, bUnhorsed);
}
