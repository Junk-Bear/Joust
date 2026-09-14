// Fill out your copyright notice in the Description page of Project Settings.


#include "Prediction/FJoustPredictionSettingsResolver.h"
#include "Attack/JoustAttackTypeDataAsset.h"
#include "Rules/JoustRuleSetDataAsset.h"
#include "Prediction/JoustPredictionTypes.h"

bool FJoustPredictionSettingsResolver::Resolve(
	const UJoustRuleSetDataAsset& InRuleSet, 
	const UJoustAttackTypeDataAsset& InAttackTypeData, 
	float InAttackerDeception, float InAttackerQuickness, 
	float InDefenderReading, FJoustPredictionSettings& OutSettings)
{
	OutSettings = FJoustPredictionSettings{};

	if (InRuleSet.DefaultInitialPredictionRadius <= 0.0f)
		return false;

	if (InAttackTypeData.InitialRadiusMultiplier <= 0.0f || InAttackTypeData.DefensePhaseDuration <= 0.0f)
		return false;

	if (!FMath::IsFinite(InAttackerDeception) || !FMath::IsFinite(InAttackerQuickness) || !FMath::IsFinite(InDefenderReading))
		return false;

	if (!ValidateStageRadiusRatios(InAttackTypeData.StageRadiusRatios))
		return false;

	OutSettings.StageCount = InAttackTypeData.StageRadiusRatios.Num();

	const int32 TransitionCount = OutSettings.StageCount - 1;

	if (TransitionCount <= 0)
		return false;

	const float RadiusModifier = ResolveRadiusModifier(InAttackerDeception, InDefenderReading);

	const float DurationModifier = ResolveDurationModifier(InAttackerQuickness);

	if (RadiusModifier <= 0.0f || DurationModifier <= 0.0f)
		return false;

	const float InitialRadius = InRuleSet.DefaultInitialPredictionRadius * InAttackTypeData.InitialRadiusMultiplier * RadiusModifier;

	const float FinalDuration = InAttackTypeData.DefensePhaseDuration * DurationModifier;

	if (!FMath::IsFinite(InitialRadius) || !FMath::IsFinite(FinalDuration) || InitialRadius <= 0.0f || FinalDuration <= 0.0f)
		return false;

	const float StageDuration = FinalDuration / static_cast<float>(TransitionCount);

	if (!FMath::IsFinite(StageDuration) || StageDuration <= 0.0f)
		return false;

	OutSettings.InitialRadius = InitialRadius;

	OutSettings.StageRadiusRatios = InAttackTypeData.StageRadiusRatios;

	OutSettings.TotalDuration = 0.0f;

	OutSettings.StageDurations.Reset();
	OutSettings.StageDurations.Reserve(TransitionCount);

	for (int32 i = 0; i < TransitionCount; ++i)
	{
		OutSettings.StageDurations.Add(StageDuration);

		OutSettings.TotalDuration += StageDuration;
	}

	OutSettings.FakeCircleCount = InAttackTypeData.FakeCircleCount;

	OutSettings.FakeRemoveStage = InAttackTypeData.FakeRemoveStage;

	return true;
}

float FJoustPredictionSettingsResolver::ResolveRadiusModifier(float InAttackerDeception, float InDefenderReading)
{
	return 1.0f + InAttackerDeception - InDefenderReading;
}

float FJoustPredictionSettingsResolver::ResolveDurationModifier(float InAttackerQuickness)
{
	return 1.0f - InAttackerQuickness;
}

bool FJoustPredictionSettingsResolver::ValidateStageRadiusRatios(const TArray<float>& InStageRadiusRatios)
{
	if (InStageRadiusRatios.Num() < 2)
		return false;

	if (!FMath::IsNearlyEqual(InStageRadiusRatios[0], 1.0f))
		return false;

	if (InStageRadiusRatios.Last() != 0.0f)
		return false;

	for (int32 i = 0; i < InStageRadiusRatios.Num(); ++i)
	{
		const float Ratio = InStageRadiusRatios[i];

		if (!FMath::IsFinite(Ratio) || Ratio < 0.0f || Ratio > 1.0f)
			return false;

		if (i > 0 && Ratio >= InStageRadiusRatios[i - 1])
			return false;
	}

	return true;
}
