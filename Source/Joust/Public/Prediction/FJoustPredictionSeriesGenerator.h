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
		const FJoustPredictionSettings& InSettings,
		const FVector2D& InTargetPoint,
		int32 InPredictionSeed,
		const FVector2D& InLanceBoxMin,
		const FVector2D& InLanceBoxMax,
		int32 InMaxRetries,
		IJoustRandomProvider& InRandomProvider,
		FJoustPredictionSeries& OutSeries);
};
