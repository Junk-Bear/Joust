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
		const FVector2D& TargetPoint,
		float InitialRadius,
		const FVector2D& LanceBoxMin, const FVector2D& LanceBoxMax,
		int32 MaxRetries,
		IJoustRandomProvider& RandomProvider,
		FJoustPredictionCircle& OutCircle);
};
