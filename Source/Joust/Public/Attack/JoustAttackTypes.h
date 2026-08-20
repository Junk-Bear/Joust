#pragma once

#include "CoreMinimal.h"
#include "Common/JoustCommonTypes.h"
#include "JoustAttackTypes.generated.h"

/**
 * Phase 2에서 확정된 공격 한 번의 데이터.
 *
 * 중요:
 * AttackPoint는 Resolve 전까지 서버 비공개 데이터, 취급 주의
 */

USTRUCT(BlueprintType)
struct JOUST_API FJoustAttackData
{
	GENERATED_BODY()


	/** 실제 공격점 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Attack")
	FVector2D AttackPoint = FVector2D::ZeroVector;

	/** 공격 종류 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Attack")
	EJoustAttackType AttackType = EJoustAttackType::Normal;

	/** 이번 공격에 적용되는 결정력 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Attack")
	float Finishing = 0.0f;

	/** 이번 공격에 적용되는 기만 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Attack")
	float Deception = 0.0f;

	/** 이번 공격에 적용되는 속공 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Attack")
	float Quickness = 0.0f;

	/** Prediction 재현용 Seed */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Attack")
	int32 PredictionSeed = 0;
};