// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Interface/JoustRandomProvider.h"
#include "Math/RandomStream.h"
#include "JoustDefaultRandomProvider.generated.h"

/**
 * 
 */
UCLASS()
class JOUST_API UJoustDefaultRandomProvider final : public UObject, public IJoustRandomProvider
{
	GENERATED_BODY()
	
public:
	/** 지정한 시드로 랜덤 스트림을 초기화 */
	void Initialize(int32 InSeed);

	// ====================
	// IJoustRandomProvider 상속됨
	// ====================
	int32 GetRandom(int32 Min, int32 Max) override;
	float GetRandom(float Min, float Max) override;
	FVector2D GetRandom(FVector2D& Min, FVector2D& Max) override;

private:
	/** 실제 랜덤 값을 생성하는 Seed 기반 스트림 */
	FRandomStream RandomStream;
};
