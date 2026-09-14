// Fill out your copyright notice in the Description page of Project Settings.


#include "Strategy/FJoustCardBanService.h"
#include "Strategy/JoustStrategyCardDataAsset.h"

void FJoustCardBanService::BeginRound(bool bInPlayerAHasBanRight, bool bInPlayerBHasBanRight)
{
	bPlayerAHasBanRight = bInPlayerAHasBanRight;
	bPlayerBHasBanRight = bInPlayerBHasBanRight;

	bPlayerABanSumitted = false;
	bPlayerBBanSumitted = false;

	AToBBannedCard = NAME_None;
	BToABannedCard = NAME_None;
}

void FJoustCardBanService::EndRound()
{
	bPlayerAHasBanRight = false;
	bPlayerBHasBanRight = false;

	bPlayerABanSumitted = false;
	bPlayerBBanSumitted = false;

	AToBBannedCard = NAME_None;
	BToABannedCard = NAME_None;
}

bool FJoustCardBanService::SubmitBan(bool bInBanningPlayerA, FName InCardID, const TArray<TObjectPtr<UJoustStrategyCardDataAsset>>&InPublicCards)
{
	if (InCardID.IsNone())
		return false;

	if (bInBanningPlayerA)
	{
		if (!bPlayerAHasBanRight || bPlayerABanSumitted)
			return false;
	}
	else
	{
		if (!bPlayerBHasBanRight || bPlayerBBanSumitted)
			return false;
	}

	if (!ContainsCardID(InPublicCards, InCardID))
		return false;

	if (bInBanningPlayerA)
	{
		AToBBannedCard = InCardID;
		bPlayerABanSumitted = true;
	}
	else
	{
		BToABannedCard = InCardID;
		bPlayerBBanSumitted = true;
	}

	return true;
}

bool FJoustCardBanService::HasPendingBan(bool bInPlayerA) const
{
	if (bInPlayerA)
	{
		return bPlayerAHasBanRight && !bPlayerABanSumitted;
	}
	else
	{
		return bPlayerBHasBanRight && !bPlayerBBanSumitted;
	}
}

bool FJoustCardBanService::AreAllRequiredBanSumitted() const
{
	const bool bPlayerACompleted = !bPlayerAHasBanRight || bPlayerABanSumitted;

	const bool bPlayerBCompleted = !bPlayerBHasBanRight || bPlayerBBanSumitted;

	return bPlayerACompleted && bPlayerBCompleted;
}

bool FJoustCardBanService::IsCardBannedForPlayer(bool bInTargetPlayerA, FName InCardID) const
{
	if (InCardID.IsNone())
		return false;

	if (bInTargetPlayerA)
	{
		return bPlayerBBanSumitted && BToABannedCard == InCardID;
	}
	else
	{
		return bPlayerABanSumitted && AToBBannedCard == InCardID;
	}
}

FName FJoustCardBanService::GetBannedCardIDForPlayer(bool bInTargetPlayerA) const
{
	if (bInTargetPlayerA)
	{
		return bPlayerBBanSumitted ? BToABannedCard : NAME_None;
	}
	else
	{
		return bPlayerABanSumitted ? AToBBannedCard : NAME_None;
	}

}

bool FJoustCardBanService::ContainsCardID(const TArray<TObjectPtr<UJoustStrategyCardDataAsset>>& InPublicCards, FName InCardID)
{
	for (const UJoustStrategyCardDataAsset* Item : InPublicCards)
	{
		if (IsValid(Item) && Item->CardID == InCardID)
		{
			return true;
		}
	}

	return false;
}
