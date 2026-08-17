// Fill out your copyright notice in the Description page of Project Settings.


#include "Strategy/FJoustCardDrawService.h"
#include "Strategy/JoustStrategyCardDataAsset.h"
#include "Interface/JoustRandomProvider.h"

bool FJoustCardDrawService::Draw(
	const TArray<TObjectPtr<UJoustStrategyCardDataAsset>>& CardPool, 
	int32 CardsToDraw, IJoustRandomProvider& RandomProvider, 
	TArray<TObjectPtr<UJoustStrategyCardDataAsset>>& OutCards)
{
	OutCards.Reset();

	if (CardsToDraw <= 0)
		return false;

	TArray<TObjectPtr<UJoustStrategyCardDataAsset>> WorkingCards;
	WorkingCards.Reserve(CardPool.Num());

	TSet<FName> SeenCardIDs;

	for (UJoustStrategyCardDataAsset* CardItem : CardPool)
	{
		if (CardItem == nullptr)
			return false;

		if (CardItem->CardID.IsNone())
			return false;

		if (SeenCardIDs.Contains(CardItem->CardID))
			return false;

		SeenCardIDs.Add(CardItem->CardID);
		WorkingCards.Add(CardItem);
	}

	if (WorkingCards.Num() < CardsToDraw)
		return false;

	OutCards.Reserve(CardsToDraw);

	while (OutCards.Num() < CardsToDraw)
	{
		const int32 RandomIdx = RandomProvider.GetRandom(0, WorkingCards.Num() - 1);

		OutCards.Add(WorkingCards[RandomIdx]);
		WorkingCards.RemoveAtSwap(RandomIdx);
	}

	return true;
}
