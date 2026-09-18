// Fill out your copyright notice in the Description page of Project Settings.


#include "Strategy/FJoustCardDrawService.h"
#include "Strategy/JoustStrategyCardDataAsset.h"
#include "Interface/JoustRandomProvider.h"

bool FJoustCardDrawService::Draw(
	const TArray<TObjectPtr<UJoustStrategyCardDataAsset>>& InCardPool, 
	int32 InCardsToDraw, IJoustRandomProvider& InRandomProvider, 
	TArray<TObjectPtr<UJoustStrategyCardDataAsset>>& OutCards)
{
	OutCards.Reset();

	if (InCardsToDraw <= 0)
		return false;

	TArray<UJoustStrategyCardDataAsset*> WorkingCards;
	WorkingCards.Reserve(InCardPool.Num());

	TSet<FName> SeenCardIDs;

	for (UJoustStrategyCardDataAsset* Item : InCardPool)
	{
		if (!IsValid(Item))
			return false;

		if (Item->CardID.IsNone())
			return false;

		if (SeenCardIDs.Contains(Item->CardID))
			return false;

		SeenCardIDs.Add(Item->CardID);
		WorkingCards.Add(Item);
	}

	if (WorkingCards.Num() < InCardsToDraw)
		return false;

	OutCards.Reserve(InCardsToDraw);

	while (OutCards.Num() < InCardsToDraw)
	{
		const int32 RandomIdx = InRandomProvider.GetRandom(0, WorkingCards.Num() - 1);

		OutCards.Add(WorkingCards[RandomIdx]);
		WorkingCards.RemoveAtSwap(RandomIdx);
	}

	return true;
}
