// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "JoustPredictionInterpolation.generated.h"

struct FJoustPredictionCircle;
struct FJoustPredictionDisplayCircle;

/**
 * 현재 프레임에 실제 표시할 원과 Stage 보간 진행도를 반환하는 클래스
 */
UCLASS()
class JOUST_API UJoustPredictionInterpolation final : public UObject
{
	GENERATED_BODY()
	
public:
	bool Interpolate(
		const FJoustPredictionCircle& CurrentCircle,
		const FJoustPredictionCircle& TargetCircle,
		float ElapsedTime,
		float StageDuration,
		FJoustPredictionDisplayCircle& OutDisplayCircle,
		float& OutAlpha) const;
};
