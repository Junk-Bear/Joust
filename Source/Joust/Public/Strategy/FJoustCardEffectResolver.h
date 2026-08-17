// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FJoustPlayerStats;
struct FJoustStrategyModifier;

/**
 * 플레이어의 스탯에 카드의 효과를 적용하는 클래스
 */
class JOUST_API FJoustCardEffectResolver final
{
public:
	static FJoustPlayerStats Resolve(
		const FJoustPlayerStats& BaseStats,
		const FJoustStrategyModifier& Modifier
	);
};
