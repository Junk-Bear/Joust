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
		const FJoustPredictionCircle& InCurrentCircle,
		float InNextRadius,
		const FVector2D& InTargetPoint,
		const FVector2D& InLanceBoxMin, const FVector2D& InLanceBoxMax,
		int32 InMaxRetries,
		IJoustRandomProvider& InRandomProvider,
		FJoustPredictionCircle& OutCircle);
};
