// Fill out your copyright notice in the Description page of Project Settings.


#include "Attack/FJoustAttackUsageTracker.h"
#include "Attack/JoustAttackTypeDataAsset.h"


bool FJoustAttackUsageTracker::Initialize(const TMap<EJoustAttackType, TObjectPtr<UJoustAttackTypeDataAsset>>& InAttackTypeSettings)
{
    RemainingUses.Reset();

    RemainingUses.Reserve(InAttackTypeSettings.Num());

    if (InAttackTypeSettings.IsEmpty())
        return false;

    for (const TPair<EJoustAttackType, TObjectPtr<UJoustAttackTypeDataAsset>>& Item : InAttackTypeSettings)
    {
        const UJoustAttackTypeDataAsset* AttackTypeDataPtr = Item.Value.Get();

        if (!IsValid(AttackTypeDataPtr))
        {
            RemainingUses.Reset();
            return false;
        }

        if (AttackTypeDataPtr->bHasUsageLimit)
        {
            if (AttackTypeDataPtr->MaxUsesPerMatch < 0)
            {
                RemainingUses.Reset();
                return false;
            }

            RemainingUses.Add(Item.Key, AttackTypeDataPtr->MaxUsesPerMatch);
        }
        else
        {
            RemainingUses.Add(Item.Key, INDEX_NONE);
        }
    }

    return true;
}

void FJoustAttackUsageTracker::Reset()
{
    RemainingUses.Reset();
}

bool FJoustAttackUsageTracker::CanUse(EJoustAttackType InAttackType) const
{
    const int32* RemainingUsesPtr = RemainingUses.Find(InAttackType);

    if (RemainingUsesPtr == nullptr)
        return false;

    if (*RemainingUsesPtr == INDEX_NONE)
        return true;

    return *RemainingUsesPtr > 0;
}

bool FJoustAttackUsageTracker::ConsumeUse(EJoustAttackType InAttackType)
{
    int32* RemainingUsesPtr = RemainingUses.Find(InAttackType);

    if (RemainingUsesPtr == nullptr)
        return false;

    if (*RemainingUsesPtr == INDEX_NONE)
        return true;

    if (*RemainingUsesPtr <= 0)
        return false;

    --(*RemainingUsesPtr);

    return true;
}

bool FJoustAttackUsageTracker::IsUnlimited(EJoustAttackType InAttackType) const
{
    const int32* RemainingUsesPtr = RemainingUses.Find(InAttackType);

    if (RemainingUsesPtr == nullptr)
        return false;

    return (RemainingUsesPtr != nullptr) && (*RemainingUsesPtr == INDEX_NONE);
}

int32 FJoustAttackUsageTracker::GetRemainingUses(EJoustAttackType InAttackType) const
{
    const int32* RemainingUsesPtr = RemainingUses.Find(InAttackType);

    if (RemainingUsesPtr == nullptr)
        return 0;

    return *RemainingUsesPtr;
}
