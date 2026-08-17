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

    if (RuleSet == nullptr)
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

    if (RuleSet == nullptr || RandomProvider == nullptr || !bMatchUsageInitialized)
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

bool UJoustAttackService::SubmitAttack(bool bPlayerA, const FJoustAttackData& InAttackData)
{
    if (RuleSet == nullptr || RandomProvider == nullptr ||
        !bMatchUsageInitialized || !bRoundPrepared || !bSubmissionOpen
        )
        return false;

    if (bPlayerA)
    {
        if (bPlayerAAttackSubmitted)
            return false;
    }
    else
    {
        if (bPlayerBAttackSubmitted)
            return false;
    }

    FJoustAttackUsageTracker& UsageTracker = bPlayerA ? PlayerAUsageTracker : PlayerBUsageTracker;

    if (!FJoustAttackValidator::Validate(*RuleSet, UsageTracker, InAttackData.AttackPoint, InAttackData.AttackType))
        return false;

    if (!UsageTracker.ConsumeUse(InAttackData.AttackType))
        return false;

    const FJoustPlayerStats& CurrentStats = bPlayerA ? PlayerACurrentStats : PlayerBCurrentStats;

    FJoustAttackData FinalAttackData = InAttackData;

    FinalAttackData.Finishing = CurrentStats.Finishing;
    FinalAttackData.Deception = CurrentStats.Deception;
    FinalAttackData.Quickness = CurrentStats.Quickness;

    FinalAttackData.PredictionSeed = RandomProvider->GetRandom(0, MAX_int32);

    if (bPlayerA)
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

bool UJoustAttackService::IsPlayerComplete(bool bPlayerA) const
{
    if (!bRoundPrepared)
        return false;

    return bPlayerA ? bPlayerAAttackSubmitted : bPlayerBAttackSubmitted;

}

bool UJoustAttackService::AreBothPlayersComplete() const
{
    return IsPlayerComplete(true) && IsPlayerComplete(false);
}

bool UJoustAttackService::CanPlayerUseAttackType(bool bPlayerA, EJoustAttackType AttackType) const
{
    if (!bMatchUsageInitialized)
        return false;

    const FJoustAttackUsageTracker& UsageTracker = bPlayerA ? PlayerAUsageTracker : PlayerBUsageTracker;

    return UsageTracker.CanUse(AttackType);
}

int32 UJoustAttackService::GetRemainingUses(bool bPlayerA, EJoustAttackType AttackType) const
{
    if (!bMatchUsageInitialized)
        return 0;

    const FJoustAttackUsageTracker& UsageTracker = bPlayerA ? PlayerAUsageTracker : PlayerBUsageTracker;

    return UsageTracker.GetRemainingUses(AttackType);
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
