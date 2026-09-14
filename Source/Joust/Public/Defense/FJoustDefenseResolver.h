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
	static FJoustDefenseResult Resolve(
		const FJoustAttackData& InAttackData,
		const FJoustDefenseData& InDefenseData,
		float InImpactTime,
		const UJoustRuleSetDataAsset& InRuleSet
	);
};
