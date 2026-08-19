// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Common/JoustCommonTypes.h"
#include "Prediction/JoustPredictionTypes.h"
#include "JoustPredictionService.generated.h"

class IJoustRandomProvider;
class UJoustRuleSetDataAsset;

/**
 * Phase3의 Prediction Circle 시스템의 진행을 맡는 클래스
 */
UCLASS()
class JOUST_API UJoustPredictionService : public UObject
{
	GENERATED_BODY()
	
public: // ########## public 함수 블록 ##########
	/** 룰셋 / 랜덤프로바이더 설정 */
	void Initialize(UJoustRuleSetDataAsset* InRuleSet, IJoustRandomProvider* InRandomProvider);

	/** 현재 공격의 세팅 / 예측원축소도 / 가짜 예측원 축소도를 생성 */
	bool PreparePrediction(
		EJoustAttackType AttackType, const FVector2D& AttackPoint,
		int32 PredictionSeed,
		float AttackerDeception, float AttackerQuickness,
		float DefenderReading);

	/** 종료시 데이터 초기화 */
	void EndRound();

private: // ########## private 함수 블록 ##########

	/** 생성된 예측원 결과만 초기화 */
	void ResetPredictionData();

private: // ########## private 변수 블록 ##########

	/** 룰셋 */
	UPROPERTY(Transient)
	TObjectPtr<UJoustRuleSetDataAsset> RuleSet = nullptr;

	/** 랜덤프로바이더 */
	IJoustRandomProvider* RandomProvider = nullptr;

	/** 예측원 세팅값 */
	FJoustPredictionSettings CurrentSettings;

	/** 예측원 축소도 */
	FJoustPredictionSeries RealSeries;

	/** 가짜 예측원 축소도 (추후 갯수가 늘어날 수도 있으니 배열형으로) */
	TArray<FJoustPredictionSeries> FakeSeries;

	/** 준비가 다 되었는지 확인 */
	bool bPrepared = false;

public: // ########## GET SET 블록 ##########

	FORCEINLINE bool IsPrepared() const { return bPrepared; }

	FORCEINLINE const FJoustPredictionSettings& GetCurrentSettings() const { return CurrentSettings; }

	FORCEINLINE const FJoustPredictionSeries& GetRealSeries() const { return RealSeries; }

	FORCEINLINE const TArray<FJoustPredictionSeries>& GetFakeSeries() const { return FakeSeries; }
};
