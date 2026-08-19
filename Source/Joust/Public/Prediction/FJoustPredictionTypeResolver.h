// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Common/JoustCommonTypes.h"

class UJoustAttackTypeDataAsset;
class UJoustRuleSetDataAsset;

/**
 * AttackType에 알맞는 AttackTypeDataAsset을 찾는 클래스
 */
class JOUST_API FJoustPredictionTypeResolver final
{
public:
	static const UJoustAttackTypeDataAsset* Resolve(
		const UJoustRuleSetDataAsset& RuleSet,
		EJoustAttackType AttackType);
};
