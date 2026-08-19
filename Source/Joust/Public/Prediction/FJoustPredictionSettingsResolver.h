// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class UJoustRuleSetDataAsset;
class UJoustAttackTypeDataAsset;

struct FJoustPredictionSettings;

/**
 * Prediction 설정을 만드는 클래스
 */
class JOUST_API FJoustPredictionSettingsResolver final
{
public: // ########## public 함수 블록 ##########

	/** 이번 공격의 최종 Prediction 설정을 계산 */
	static bool Resolve(
		const UJoustRuleSetDataAsset& RuleSet,
		const UJoustAttackTypeDataAsset& AttackTypeData,
		float AttackerDeception, float AttackerQuickness,
		float DefenderReading,
		FJoustPredictionSettings& OutSettings
	);

private: // ########## private 함수 블록 ##########

	/** 스탯에 따른 InitialRadius에 적용 배율 계산 */
	static float ResolveRadiusModifier(float AttackerDeception, float DefenderReading);

	/** 스탯에 따른 Duration 적용 배율 계산 */
	static float ResolveDurationModifier(float AttackerQuickness);

	/** 입력 설정 데이터의 불변식 검사 */
	static bool ValidateStageRadiusRatios(const TArray<float>& StageRadiusRatios);
};
