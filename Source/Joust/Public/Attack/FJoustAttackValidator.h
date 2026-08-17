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
		const UJoustRuleSetDataAsset& RuleSet,
		const FJoustAttackUsageTracker& UsageTracker,
		const FVector2D& AttackPoint,
		EJoustAttackType AttackType
	);
};
