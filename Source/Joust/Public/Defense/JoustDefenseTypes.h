#pragma once

#include "CoreMinimal.h"
#include "Common/JoustCommonTypes.h"
#include "JoustDefenseTypes.generated.h"

/**
* 수비자가 실제 제출한 입력 데이터
* 
* 아직 성공/실패 판정X
*/
USTRUCT(BlueprintType)
struct JOUST_API FJoustDefenseData
{
	GENERATED_BODY()

	/**
	* 수비 플레이어 식별값
	* 
	* 0 = Player A
	* 1 = Player B
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Defense")
	int32 DefenderId = INDEX_NONE;

	/** 최종 방패 위치 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Defense")
	FVector2D ShieldPoint = FVector2D::ZeroVector;

	/** 패링 입력을 했는지 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Defense")
	bool bParryAttempted = false;

	/** 패링 입력한 타이밍(시각) */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Defense")
	double ParryInputTime = 0.0;

	/** 이번 방어에 적용된 버팀 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Defense")
	float Stability = 0.0f;

	/** 이번 방어에 적용된 판독 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Defense")
	float Reading = 0.0f;

	/** 이번 방어에 적용된 방패 기동성*/
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Defense")
	float ShieldMobility = 0.0f;

	/** 이번 방어에 적용된 패링 감각 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Defense")
	float ParrySense = 0.0f;
};

/**
* 계산을 끝낸 수비 판정 결과
*/
USTRUCT(BlueprintType)
struct JOUST_API FJoustDefenseResult
{
	GENERATED_BODY()

	/** 판정된 가드존 구역 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Defense")
	EJoustGuardZone GuardZone = EJoustGuardZone::Outside;

	/** AttackPoint와 ShieldPoint 사이의 거리 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Defense")
	float HitDistance = 0.0f;

	/** 최종 방어 행동 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Defense")
	EJoustDefenseAction DefenseAction = EJoustDefenseAction::Guard;

	/** 패링 판정 결과 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Defense")
	EJoustParryOutcome ParryOutcome = EJoustParryOutcome::NotAttempted;

	/** 이상적인 패링 타이밍(시각)과 실제 입력 타이밍(시각)의 차이 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Defense")
	float ParryTimingError = 0.0f;

	/** 이번 상황에서 허용된 패링 윈도우 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Defense")
	float AllowedParryWindow = 0.0f;

	/** Edge Parry 판정 여부 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Defense")
	bool bIsEdgeParry = false;

	/** 결정타 확률에 적용되는 방어 배율 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Defense")
	float CriticalDefenseMultiplier = 1.0f;

	/** 상대의 득점을 차단했는지 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Defense")
	bool bBlockedScore = false;
};