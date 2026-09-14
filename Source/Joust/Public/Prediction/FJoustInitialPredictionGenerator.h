// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FJoustPredictionCircle;

class IJoustRandomProvider;

/**
 * 최초원을 만드는 클래스
 */
class JOUST_API FJoustInitialPredictionGenerator final
{
public:
	static bool Generate(
		const FVector2D& InTargetPoint,
		float InInitialRadius,
		const FVector2D& InLanceBoxMin, const FVector2D& InLanceBoxMax,
		int32 InMaxRetries,
		IJoustRandomProvider& InRandomProvider,
		FJoustPredictionCircle& OutCircle);
};
