// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Common/JoustCommonTypes.h"

class FJoustAttackUsageTracker;
class UJoustRuleSetDataAsset;

/**
 * 공격에 사용된 Point와 Type, 횟수등을 검증
 */
class JOUST_API FJoustAttackValidator
{
public:
	static bool Validate(
		const UJoustRuleSetDataAsset& InRuleSet,
		const FJoustAttackUsageTracker& InUsageTracker,
		const FVector2D& InAttackPoint,
		EJoustAttackType InAttackType
	);
};
