// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FJoustPredictionSeries;
struct FJoustPredictionSettings;

class IJoustRandomProvider;

/**
 * PredictionSeries를 만드는 클래스
 */
class JOUST_API FJoustPredictionSeriesGenerator final
{
public:
	static bool Generate(
		const FJoustPredictionSettings& Settings,
		const FVector2D& TargetPoint,
		int32 PredictionSeed,
		const FVector2D& LanceBoxMin,
		const FVector2D& LanceBoxMax,
		int32 MaxRetries,
		IJoustRandomProvider& RandomProvider,
		FJoustPredictionSeries& OutSeries);
};
