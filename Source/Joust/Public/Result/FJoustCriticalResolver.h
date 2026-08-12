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

	/** 결정타 계산 & 낙마 판정 */
	static bool Resolve(
		float BaseCriticalChance, float MaxCriticalChance,
		float AttackTypeMultiplier, float DefenseMultiplier, float AttackerFinishing, float DefenderStability, 
		IJoustRandomProvider& RandomProvider, float& OutFinalCriticalChance
	);
};
