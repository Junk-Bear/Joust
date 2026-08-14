// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "JoustRandomProvider.generated.h"


UINTERFACE(MinimalAPI)
class UJoustRandomProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * 랜덤 값이 필요하다고 계약만 선언
 */
class JOUST_API IJoustRandomProvider
{
	GENERATED_BODY()

public:
	/** Min~Max 범위의 랜덤 정수를 반환 */
	virtual int32 GetRandom(int32 Min, int32 Max) = 0;

	/** Min~Max 범위의 랜덤 실수를 반환 */
	virtual float GetRandom(float Min, float Max) = 0;

	/** Min~Max 범위의 랜덤 실수를 반환 */
	virtual FVector2D GetRandom(FVector2D& Min, FVector2D& Max) = 0;
};
