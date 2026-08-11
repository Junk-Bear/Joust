#pragma once

#include "CoreMinimal.h"
#include "JoustStrategyTypes.generated.h"

/**
 * 전략 카드 한 장이 능력치에 주는 변화량
 */
USTRUCT(BlueprintType)
struct JOUST_API FJoustStrategyModifier
{
	GENERATED_BODY()

	/** 결정력 보정 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Strategy")
	float FinishingModifier = 0.0f;

	/** 기만 보정 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Strategy")
	float DeceptionModifier = 0.0f;

	/** 속공 보정 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Strategy")
	float QuicknessModifier = 0.0f;

	/** 버팀 보정 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Strategy")
	float StabilityModifier = 0.0f;

	/** 판독 보정 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Strategy")
	float ReadingModifier = 0.0f;

	/** 방패 기동성 보정 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Strategy")
	float ShieldMobilityModifier = 0.0f;

	/** 패링 감각 보정 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Strategy")
	float ParrySenseModifier = 0.0f;
};