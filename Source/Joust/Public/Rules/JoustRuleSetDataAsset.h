// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Common/JoustCommonTypes.h"
#include "JoustRuleSetDataAsset.generated.h"

class UJoustStrategyCardDataAsset;
class UJoustAttackTypeDataAsset;

/**
 * 경기 전체 공통 룰 & 기본 밸런스 보관용
 */
UCLASS(BlueprintType)
class JOUST_API UJoustRuleSetDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:

	// ====================
	// Match
	// ====================

	/** 기본 정규 라운드 수 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Rules|Match", meta = (ClampMin = "1"))
	int32 BaseRoundCount = 5;

	/** 연장전을 허용 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Rules|Match")
	bool bContinueOvertimeUntilWinner = true;

	// ====================
	// Phase
	// ====================
	/** Phase 1 기본 제한 시간 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Phase", meta = (ClampMin = "0.0"))
	float StrategyPhaseDuration = 20.0f;

	/** Phase 2 기본 제한 시간*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Phase", meta = (ClampMin = "0.0"))
	float AttackPhaseDuration = 15.0f;

	/** 한쪽 플레이어의 선택 완료시 제한시간 단축 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Phase", meta = (ClampMin = "0.0"))
	float OnePlayerCompletedRemainingTime = 7.0f;

	// ====================
	// Strategy
	// ====================

	/** 전략 카드 풀 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Rules|Strategy")
	TArray<TObjectPtr<UJoustStrategyCardDataAsset>> StrategyCardPool;

	/** 매 라운드 양쪽에게 공통 공개할 카드 수 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Rules|Strategy", meta = (ClampMin = "1"))
	int32 CardShownPerRound = 5;

	/** Edge Parry로 봉인할 수 있는 최대 카드 수 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Rules|Strategy", meta = (ClampMin = "0"))
	int32 MaxCardBansPerPlayer = 1;

	// ====================
	// Attack
	// ====================

	/** 찌르기 타입에 관한 것*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Rules|Attack")
	TMap<EJoustAttackType, TObjectPtr<UJoustAttackTypeDataAsset>> AttackTypeSettings;

	// ====================
	// Lance Box
	// ====================

	/** 공격 지점으로 선택 가능한 최소 좌표 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Rules|LanceBox")
	FVector2D LanceBoxMin = FVector2D(-1.0f, -1.0f);

	/** 공격 지점으로 선택 가능한 최대 좌표 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Rules|LanceBox")
	FVector2D LanceBoxMax = FVector2D(1.0f, 1.0f);

	// ====================
	// Guard
	// ====================

	/** 퍼펙트존 판정 반지름 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Rules|Guard", meta = (ClampMin = "0.0"))
	float PerfectZoneRadius = 0.12f;

	/** 굿존 판정 반지름 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Rules|Guard", meta = (ClampMin = "0.0"))
	float GoodZoneRadius = 0.30f;

	/** 배드존 판정 반지름 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Rules|Guard", meta = (ClampMin = "0.0"))
	float BadZoneRadius = 0.50f;

	// ====================
	// Parry
	// ====================

	/** 퍼펙트존 패링 허용 시간 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Rules|Parry", meta = (ClampMin = "0.0"))
	float PerfectZoneParryWindow = 0.12f;

	/** 굿존 패링 허용 시간 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Rules|Parry", meta = (ClampMin = "0.0"))
	float GoodZoneParryWindow = 0.09f;

	/** 배드존 패링 허용 시간 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Rules|Parry", meta = (ClampMin = "0.0"))
	float BadZoneParryWindow = 0.06f;

	// ====================
	// Critical
	// ====================

	/** 기본 결정타 확률 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Rules|Critical", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float BaseCriticalChance = 0.03f;
	
	/** 기본 결정타 확률 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Rules|Critical", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MaxCriticalChance = 0.10f;

	// ====================
	// 가드시 결정타 배율
	// ====================

	/** 퍼펙트존 가드시 결정타 배율 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Rules|Critical|Guard", meta = (ClampMin = "0.0"))
	float GuardPerfectCriticalMultiplier = 0.0f;

	/** 굿존 가드시 결정타 배율 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Rules|Critical|Guard", meta = (ClampMin = "0.0"))
	float GuardGoodCriticalMultiplier = 0.5f;

	/** 배드존 가드시 결정타 배율 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Rules|Critical|Guard", meta = (ClampMin = "0.0"))
	float GuardBadCriticalMultiplier = 1.0f;

	/** 가드 실패시 결정타 배율 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Rules|Critical|Guard", meta = (ClampMin = "0.0"))
	float GuardOutsideCriticalMultiplier = 1.5f;

	// ====================
	// 패링 실패시 결정타 배율
	// ====================

	/** 퍼펙트존에서 패링 실패시 결정타 배율 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Rules|Critical|ParryFailure", meta = (ClampMin = "0.0"))
	float ParryFailurePerfectCriticalMultiplier = 0.5f;

	/** 굿존에서 패링 실패시 결정타 배율 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Rules|Critical|ParryFailure", meta = (ClampMin = "0.0"))
	float ParryFailureGoodCriticalMultiplier = 1.0f;

	/** 배드존에서 패링 실패시 결정타 배율 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Rules|Critical|ParryFailure", meta = (ClampMin = "0.0"))
	float ParryFailureBadCriticalMultiplier = 1.5f;


	// ====================
	// Prediction
	// ====================

	/** 기본 최초 예측원 반지름 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Rules|Critical|Prediction", meta = (ClampMin = "0.0"))
	float DefaultInitialPredictionRadius = 1.0f;

	/**
	* 유효한 Prediction Circle 생성에 실패 했을 때 Generator가 재시도할 최대 횟 수
	* 
	* 나중에 로그 확인용
	* 최대 횟수 이상 걸리면 버그 터질까봐 안전장치용
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Rules|Critical|Prediction", meta = (ClampMin = "1"))
	int32 MaxPredictionGenerationAttempts = 32;
};
