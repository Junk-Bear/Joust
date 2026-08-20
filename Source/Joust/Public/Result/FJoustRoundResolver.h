// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FJoustRoundResult;
struct FJoustAttackData;
struct FJoustDefenseData;

class UJoustRuleSetDataAsset;
class IJoustRandomProvider;

/**
 * RoundResult를 만드는 클래스
 */
class JOUST_API FJoustRoundResolver final
{
public:
	static FJoustRoundResult Resolve(
		int32 RoundNumber,
		const FJoustAttackData& PlayerAAttackData, const FJoustDefenseData& PlayerBDefenseData,	float PlayerAToBImpactTime,
		const FJoustAttackData& PlayerBAttackData, const FJoustDefenseData& PlayerADefenseData,	float PlayerBToAImpactTime,
		const UJoustRuleSetDataAsset& RuleSet, IJoustRandomProvider& RandomProvider
	);
};
