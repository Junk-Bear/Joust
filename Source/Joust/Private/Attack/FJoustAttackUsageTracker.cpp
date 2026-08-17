// Fill out your copyright notice in the Description page of Project Settings.


#include "Attack/FJoustAttackUsageTracker.h"
#include "Attack/JoustAttackTypeDataAsset.h"


bool FJoustAttackUsageTracker::Initialize(const TMap<EJoustAttackType, TObjectPtr<UJoustAttackTypeDataAsset>>& AttackTypeSettings)
{
    RemainingUses.Reset();

    RemainingUses.Reserve(AttackTypeSettings.Num());

    if (AttackTypeSettings.IsEmpty())
        return false;

    for (const TPair<EJoustAttackType, TObjectPtr<UJoustAttackTypeDataAsset>>& PairItem : AttackTypeSettings)
    {
        const UJoustAttackTypeDataAsset* AttackTypeData = PairItem.Value.Get();

        if (AttackTypeData == nullptr)
        {
            RemainingUses.Reset();
            return false;
        }

        if (AttackTypeData->bHasUsageLimit)
        {
            if (AttackTypeData->MaxUsesPerMatch < 0)
            {
                RemainingUses.Reset();
                return false;
            }

            RemainingUses.Add(PairItem.Key, AttackTypeData->MaxUsesPerMatch);
        }
        else
        {
            RemainingUses.Add(PairItem.Key, INDEX_NONE);
        }
    }

    return true;
}

void FJoustAttackUsageTracker::Reset()
{
    RemainingUses.Reset();
}

bool FJoustAttackUsageTracker::CanUse(EJoustAttackType AttackType) const
{
    const int32* Remaining = RemainingUses.Find(AttackType);

    if (Remaining == nullptr)
        return false;

    if (*Remaining == INDEX_NONE)
        return false;

    return *Remaining > 0;
}

bool FJoustAttackUsageTracker::ConsumeUse(EJoustAttackType AttackType)
{
    int32* Remaining = RemainingUses.Find(AttackType);

    if (Remaining == nullptr)
        return false;

    if (*Remaining == INDEX_NONE)
        return true;

    if (*Remaining <= 0)
        return false;

    --(*Remaining);

    return true;
}

bool FJoustAttackUsageTracker::IsUnlimited(EJoustAttackType AttackType) const
{
    const int32* Remaining = RemainingUses.Find(AttackType);

    if (Remaining == nullptr)
        return false;

    return (Remaining != nullptr) && (*Remaining == INDEX_NONE);
}

int32 FJoustAttackUsageTracker::GetRemainingUses(EJoustAttackType AttackType) const
{
    const int32* Remaining = RemainingUses.Find(AttackType);

    if (Remaining == nullptr)
        return 0;

    return *Remaining;
}
