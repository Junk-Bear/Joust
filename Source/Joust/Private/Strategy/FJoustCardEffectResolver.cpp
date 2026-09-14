// Fill out your copyright notice in the Description page of Project Settings.


#include "Strategy/FJoustCardEffectResolver.h"
#include "Player/JoustPlayerTypes.h"
#include "Strategy/JoustStrategyTypes.h"

FJoustPlayerStats FJoustCardEffectResolver::Resolve(const FJoustPlayerStats& InBaseStats, const FJoustStrategyModifier& InModifier)
{
	FJoustPlayerStats Result = InBaseStats;

	Result.Finishing += InModifier.FinishingModifier;
	Result.Deception += InModifier.DeceptionModifier;
	Result.Quickness += InModifier.QuicknessModifier;
	Result.Stability += InModifier.StabilityModifier;
	Result.Reading += InModifier.ReadingModifier;
	Result.ShieldMobility += InModifier.ShieldMobilityModifier;
	Result.ParrySense += InModifier.ParrySenseModifier;

	return Result;
}
