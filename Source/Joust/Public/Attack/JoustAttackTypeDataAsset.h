// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Common/JoustCommonTypes.h"
#include "JoustAttackTypeDataAsset.generated.h"

class UTexture2D;

/**
 * 찌르기 타입 원본 데이터
 */
UCLASS(BlueprintType)
class JOUST_API UJoustAttackTypeDataAsset : public UDataAsset
{
	GENERATED_BODY()

public: // 생성자용
	UJoustAttackTypeDataAsset();

public:

	/** UI 표시 이름 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Attack")
	FText DisplayName;

	/** UI 설명 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Attack", meta = (MultiLine = "true"))
	FText Description;

	/** UI 아이콘 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Attack")
	TObjectPtr<UTexture2D> Icon = nullptr;

	// ====================
	// 사용 횟수에 관한 것
	// ====================

	/** 사용 횟수 제한이 존재하는지 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Attack|Usage")
	bool bHasUsageLimit = false;

	/** 
	* 경기에서 최대 사용 횟수
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Attack|Usage", meta = (ClampMin = "0", EditCondition = "bHasUsageLimit"))
	int32 MaxUsesPerMatch = 0;

	// ====================
	// 결정타 / 낙마 관련
	// ====================

	/** AttackType에 대한 결정타 보정값 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Attack|Critical", meta = (ClampMin = "0.0"))
	float CriticalChanceMultiplier = 1.0f;

	// ====================
	// Defense 
	// ====================

	/**
	* 해당 타입을 수비할 수 있는 기본 시간
	* 
	* Normal / Strong / Trick / Slow
	* 1.3 / 0.9 / 1.3 / 1.8
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Attack|Defense", meta = (ClampMin = "0.01"))
	float DefensePhaseDuration = 1.3f;
	
	// ====================
	// 예측원
	// ====================
	
	/** 최초 예측원 크기 배율 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Attack|Prediction", meta = (ClampMin = "0.0"))
	float InitialRadiusMultiplier = 1.0f;

	/** 
	* 공격 타입 고유 Prediction Radius 곡선
	* 
	* 정규화 비율로 저장
	* 
	* Normal & Trick : 
	* 1.00 -> 0.75 -> 0.50 -> 0.25 -> 0.00
	* 
	* Strong : 
	* 1.00 -> 0.60 -> 0.20 -> 0.00
	* 
	* Slow
	* 1.00 -> 0.60 -> 0.35 -> 0.20 -> 0.15 -> 0.10 -> 0.00
	* 
	* 0.00 = 실제 AttackPoint
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Attack|Prediction")
	TArray<float> StageRadiusRatios;



	// ====================
	// 가짜원 용
	// ====================

	/** Trick공격에서 생성할 가짜 원 수 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Attack|Fake", meta = (ClampMin = "0"))
	int32 FakeCircleCount = 0;

	/** 
	*가짜 원이 제거되는 Stage
	* INDEX_NONE : 제거 별도 지정X
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Attack|Fake", meta = (EditCondition = "FakeCircleCount > 0", EditConditionHides))
	int32 FakeRemoveStage = INDEX_NONE;

};
