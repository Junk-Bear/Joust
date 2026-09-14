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

void AJoustPlayerState::SetRemainingAttackUses(EJoustAttackType InAttackType, int32 InRemainingUses)
{
	RemainingAttackUses.Add(InAttackType, InRemainingUses);

	for (FJoustAttackUsageState& Item : ReplicatedAttackUsageStates)
	{
		if (Item.AttackType == InAttackType)
		{
			Item.RemainingUses = InRemainingUses;

			return;
		}
	}

	FJoustAttackUsageState& NewUsageStateRef = ReplicatedAttackUsageStates.AddDefaulted_GetRef();
	NewUsageStateRef.AttackType = InAttackType;
	NewUsageStateRef.RemainingUses = InRemainingUses;
}

int32 AJoustPlayerState::GetRemainingAttackUses(EJoustAttackType InAttackType) const
{
	const int32* RemainingUsesPtr = RemainingAttackUses.Find(InAttackType);

	return RemainingUsesPtr != nullptr ? *RemainingUsesPtr : 0;
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

	for (const TPair<EJoustAttackType, int32>& Item : InRemainingAttackUses)
	{
		SetRemainingAttackUses(Item.Key,Item.Value);
	}

	ReplicatedAttackUsageStates.Sort([](const FJoustAttackUsageState& InA,const FJoustAttackUsageState& InB)
		{
			return static_cast<uint8>(InA.AttackType) < static_cast<uint8>(InB.AttackType);
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
