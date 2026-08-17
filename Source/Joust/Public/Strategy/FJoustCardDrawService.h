// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class UJoustStrategyCardDataAsset;
class IJoustRandomProvider;

/**
 * 카드 덱에서 카드 5장을 뽑는 클래스
 */
class JOUST_API FJoustCardDrawService final
{
public:
	static bool Draw(
		const TArray<TObjectPtr<UJoustStrategyCardDataAsset>>& CardPool,
		int32 CardsToDraw,
		IJoustRandomProvider& RandomProvider,
		TArray<TObjectPtr<UJoustStrategyCardDataAsset>>& OutCards
	);
};
