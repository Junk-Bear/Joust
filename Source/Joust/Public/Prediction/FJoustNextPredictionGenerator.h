// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FJoustPredictionCircle;
class IJoustRandomProvider;

/**
 * 다음 예측원을 만드는 클래스
 */
class JOUST_API FJoustNextPredictionGenerator final
{
public:
	static bool Generate(
		const FJoustPredictionCircle& CurrentCircle,
		float NextRadius,
		const FVector2D& TargetPoint,
		const FVector2D& LanceBoxMin, const FVector2D& LanceBoxMax,
		int32 MaxRetries,
		IJoustRandomProvider& RandomProvider,
		FJoustPredictionCircle& OutCircle);
};
