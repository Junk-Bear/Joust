// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FJoustDefenseResult;
struct FJoustAttackData;
struct FJoustDefenseData;

class UJoustRuleSetDataAsset;

/**
 * DefenseRusult를 만드는 클래스
 */
class JOUST_API FJoustDefenseResolver final
{
public:
	/** DefenseResult를 만들고 반환함 */
	static FJoustDefenseResult Resolve(
		const FJoustAttackData& AttackData,
		const FJoustDefenseData& DefenseData,
		float impactTime,
		const UJoustRuleSetDataAsset& RuleSet
	);
};
