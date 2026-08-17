// Fill out your copyright notice in the Description page of Project Settings.


#include "Strategy/FJoustCardEffectResolver.h"
#include "Player/JoustPlayerTypes.h"
#include "Strategy/JoustStrategyTypes.h"

FJoustPlayerStats FJoustCardEffectResolver::Resolve(const FJoustPlayerStats& BaseStats, const FJoustStrategyModifier& Modifier)
{
	FJoustPlayerStats Result = BaseStats;

	Result.Finishing += Modifier.FinishingModifier;
	Result.Deception += Modifier.DeceptionModifier;
	Result.Quickness += Modifier.QuicknessModifier;
	Result.Stability += Modifier.StabilityModifier;
	Result.Reading += Modifier.ReadingModifier;
	Result.ShieldMobility += Modifier.ShieldMobilityModifier;
	Result.ParrySense += Modifier.ParrySenseModifier;

	return Result;
}
