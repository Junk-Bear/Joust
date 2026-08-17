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

bool FJoustCardBanService::SubmitBan(bool bBanningPlayerA, FName CardID, const TArray<TObjectPtr<UJoustStrategyCardDataAsset>>&PublicCards)
{
	if (CardID.IsNone())
		return false;

	if (bBanningPlayerA)
	{
		if (!bPlayerAHasBanRight || bPlayerABanSumitted)
			return false;
	}
	else
	{
		if (!bPlayerAHasBanRight || bPlayerBBanSumitted)
			return false;
	}

	if (!ContainsCardID(PublicCards, CardID))
		return false;

	if (bBanningPlayerA)
	{
		AToBBannedCard = CardID;
		bPlayerABanSumitted = true;
	}
	else
	{
		BToABannedCard = CardID;
		bPlayerBBanSumitted = true;
	}

	return true;
}

bool FJoustCardBanService::HasPendingBan(bool bPlayerA) const
{
	if (bPlayerA)
	{
		return bPlayerABanSumitted && !bPlayerABanSumitted;
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

bool FJoustCardBanService::IsCardBannedForPlayer(bool bTargetPlayerA, FName CardID) const
{
	if (CardID.IsNone())
		return false;

	if (bTargetPlayerA)
	{
		return bPlayerBBanSumitted && BToABannedCard == CardID;
	}
	else
	{
		return bPlayerABanSumitted && AToBBannedCard == CardID;
	}
}

FName FJoustCardBanService::GetBannedCardIDForPlayer(bool bTargetPlayerA) const
{
	if (bTargetPlayerA)
	{
		return bPlayerBBanSumitted ? BToABannedCard : NAME_None;
	}
	else
	{
		return bPlayerABanSumitted ? AToBBannedCard : NAME_None;
	}

}

bool FJoustCardBanService::ContainsCardID(const TArray<TObjectPtr<UJoustStrategyCardDataAsset>>& PublicCards, FName CardID)
{
	for (const UJoustStrategyCardDataAsset* CardItem : PublicCards)
	{
		if (CardItem != nullptr && CardItem->CardID == CardID)
		{
			return true;
		}
	}

	return false;
}
