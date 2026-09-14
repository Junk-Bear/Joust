// Fill out your copyright notice in the Description page of Project Settings.


#include "Attack/JoustAttackService.h"
#include "Rules/JoustRuleSetDataAsset.h"
#include "Interface/JoustRandomProvider.h"
#include "Attack/FJoustAttackValidator.h"


bool UJoustAttackService::Initialize(UJoustRuleSetDataAsset* InRuleSet, IJoustRandomProvider& InRandomProvider)
{
    RuleSet = InRuleSet;
    RandomProvider = &InRandomProvider;

    return ResetMatchUsage();
}

bool UJoustAttackService::ResetMatchUsage()
{
    ResetRoundState();

    PlayerAUsageTracker.Reset();
    PlayerBUsageTracker.Reset();

    bMatchUsageInitialized = false;

    if (!IsValid(RuleSet))
        return false;

    if (!PlayerAUsageTracker.Initialize(RuleSet->AttackTypeSettings))
        return false;

    if (!PlayerBUsageTracker.Initialize(RuleSet->AttackTypeSettings))
        return false;

    bMatchUsageInitialized = true;

    return true;
}

bool UJoustAttackService::PrepareRound(const FJoustPlayerStats& InPlayerACurrentStats, const FJoustPlayerStats& InPlayerBCurrentStats)
{
    ResetRoundState();

    if (!IsValid(RuleSet) || RandomProvider == nullptr || !bMatchUsageInitialized)
        return false;

    PlayerACurrentStats = InPlayerACurrentStats;
    PlayerBCurrentStats = InPlayerBCurrentStats;

    bRoundPrepared = true;

    return true;
}

void UJoustAttackService::EndRound()
{
    ResetRoundState();
}

bool UJoustAttackService::SubmitAttack(bool bInPlayerA, const FJoustAttackData& InAttackData)
{
    if (!IsValid(RuleSet) || RandomProvider == nullptr ||
        !bMatchUsageInitialized || !bRoundPrepared || !bSubmissionOpen
        )
        return false;

    if (bInPlayerA)
    {
        if (bPlayerAAttackSubmitted)
            return false;
    }
    else
    {
        if (bPlayerBAttackSubmitted)
            return false;
    }

    FJoustAttackUsageTracker& UsageTrackerRef = bInPlayerA ? PlayerAUsageTracker : PlayerBUsageTracker;

    if (!FJoustAttackValidator::Validate(*RuleSet, UsageTrackerRef, InAttackData.AttackPoint, InAttackData.AttackType))
        return false;

    if (!UsageTrackerRef.ConsumeUse(InAttackData.AttackType))
        return false;

    const FJoustPlayerStats& CurrentStatsRef = bInPlayerA ? PlayerACurrentStats : PlayerBCurrentStats;

    FJoustAttackData FinalAttackData = InAttackData;

    FinalAttackData.Finishing = CurrentStatsRef.Finishing;
    FinalAttackData.Deception = CurrentStatsRef.Deception;
    FinalAttackData.Quickness = CurrentStatsRef.Quickness;

    FinalAttackData.PredictionSeed = RandomProvider->GetRandom(0, MAX_int32);

    if (bInPlayerA)
    {
        PlayerAAttackData = FinalAttackData;

        bPlayerAAttackSubmitted = true;
    }
    else
    {
        PlayerBAttackData = FinalAttackData;

        bPlayerBAttackSubmitted = true;
    }

    return true;
}

bool UJoustAttackService::IsPlayerComplete(bool bInPlayerA) const
{
    if (!bRoundPrepared)
        return false;

    return bInPlayerA ? bPlayerAAttackSubmitted : bPlayerBAttackSubmitted;

}

bool UJoustAttackService::AreBothPlayersComplete() const
{
    return IsPlayerComplete(true) && IsPlayerComplete(false);
}

bool UJoustAttackService::CanPlayerUseAttackType(bool bInPlayerA, EJoustAttackType InAttackType) const
{
    if (!bMatchUsageInitialized)
        return false;

    const FJoustAttackUsageTracker& UsageTrackerRef = bInPlayerA ? PlayerAUsageTracker : PlayerBUsageTracker;

    return UsageTrackerRef.CanUse(InAttackType);
}

int32 UJoustAttackService::GetRemainingUses(bool bInPlayerA, EJoustAttackType InAttackType) const
{
    if (!bMatchUsageInitialized)
        return 0;

    const FJoustAttackUsageTracker& UsageTrackerRef = bInPlayerA ? PlayerAUsageTracker : PlayerBUsageTracker;

    return UsageTrackerRef.GetRemainingUses(InAttackType);
}

bool UJoustAttackService::GetUsageSnapshot(bool bInPlayerA, TMap<EJoustAttackType, int32>& OutRemainingUses) const
{
    OutRemainingUses.Reset();

    if (!IsValid(RuleSet) || !bMatchUsageInitialized)
        return false;

    OutRemainingUses.Reserve(
        RuleSet->AttackTypeSettings.Num());

    const FJoustAttackUsageTracker& UsageTrackerRef = bInPlayerA ? PlayerAUsageTracker : PlayerBUsageTracker;

    for (const TPair<EJoustAttackType,TObjectPtr<UJoustAttackTypeDataAsset>>& Item : RuleSet->AttackTypeSettings)
    {
        OutRemainingUses.Add(Item.Key, UsageTrackerRef.GetRemainingUses(Item.Key));
    }

    return true;
}

void UJoustAttackService::ResetRoundState()
{
    PlayerACurrentStats = FJoustPlayerStats{};
    PlayerBCurrentStats = FJoustPlayerStats{};

    PlayerAAttackData = FJoustAttackData{};
    PlayerBAttackData = FJoustAttackData{};

    bRoundPrepared = false;
    
    bSubmissionOpen = false;

    bPlayerAAttackSubmitted = false;
    bPlayerBAttackSubmitted = false;
}
