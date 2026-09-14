// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class IJoustRandomProvider;

/**
 * 최종 결정타 확률을 계산하고 낙마가 발생했는지 판정함.
 */
class JOUST_API FJoustCriticalResolver final
{
public:
	static bool Resolve(
		float InBaseCriticalChance, float InMaxCriticalChance,
		float InAttackTypeMultiplier, float InDefenseMultiplier, float InAttackerFinishing, float InDefenderStability, 
		IJoustRandomProvider& InRandomProvider, float& OutFinalCriticalChance
	);
};
