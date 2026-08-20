// Fill out your copyright notice in the Description page of Project Settings.


#include "Prediction/FJoustPredictionSettingsResolver.h"
#include "Attack/JoustAttackTypeDataAsset.h"
#include "Rules/JoustRuleSetDataAsset.h"
#include "Prediction/JoustPredictionTypes.h"

bool FJoustPredictionSettingsResolver::Resolve(
	const UJoustRuleSetDataAsset& RuleSet, 
	const UJoustAttackTypeDataAsset& AttackTypeData, 
	float AttackerDeception, float AttackerQuickness, 
	float DefenderReading, FJoustPredictionSettings& OutSettings)
{
	OutSettings = FJoustPredictionSettings{};

	if (RuleSet.DefaultInitialPredictionRadius <= 0.0f)
		return false;

	if (AttackTypeData.InitialRadiusMultiplier <= 0.0f || AttackTypeData.DefensePhaseDuration <= 0.0f)
		return false;

	if (!FMath::IsFinite(AttackerDeception) || !FMath::IsFinite(AttackerQuickness) || !FMath::IsFinite(DefenderReading))
		return false;

	if (!ValidateStageRadiusRatios(AttackTypeData.StageRadiusRatios))
		return false;

	OutSettings.StageCount = AttackTypeData.StageRadiusRatios.Num();

	const int32 TransitionCount = OutSettings.StageCount - 1;

	if (TransitionCount <= 0)
		return false;

	const float RadiusModifier = ResolveRadiusModifier(AttackerDeception, DefenderReading);

	const float DurationModifier = ResolveDurationModifier(AttackerQuickness);

	if (RadiusModifier <= 0.0f || DurationModifier <= 0.0f)
		return false;

	const float InitialRadius = RuleSet.DefaultInitialPredictionRadius * AttackTypeData.InitialRadiusMultiplier * RadiusModifier;

	const float FinalDuration = AttackTypeData.DefensePhaseDuration * DurationModifier;

	if (!FMath::IsFinite(InitialRadius) || !FMath::IsFinite(FinalDuration) || InitialRadius <= 0.0f || FinalDuration <= 0.0f)
		return false;

	const float StageDuration = FinalDuration / static_cast<float>(TransitionCount);

	if (!FMath::IsFinite(StageDuration) || StageDuration <= 0.0f)
		return false;

	OutSettings.InitialRadius = InitialRadius;

	OutSettings.StageRadiusRatios = AttackTypeData.StageRadiusRatios;

	OutSettings.TotalDuration = 0.0f;

	OutSettings.StageDurations.Reset();
	OutSettings.StageDurations.Reserve(TransitionCount);

	for (int32 i = 0; i < TransitionCount; ++i)
	{
		OutSettings.StageDurations.Add(StageDuration);

		OutSettings.TotalDuration += StageDuration;
	}

	OutSettings.FakeCircleCount = AttackTypeData.FakeCircleCount;

	OutSettings.FakeRemoveStage = AttackTypeData.FakeRemoveStage;

	return true;
}

float FJoustPredictionSettingsResolver::ResolveRadiusModifier(float AttackerDeception, float DefenderReading)
{
	return 1.0f + AttackerDeception - DefenderReading;
}

float FJoustPredictionSettingsResolver::ResolveDurationModifier(float AttackerQuickness)
{
	return 1.0f - AttackerQuickness;
}

bool FJoustPredictionSettingsResolver::ValidateStageRadiusRatios(const TArray<float>& StageRadiusRatios)
{
	if (StageRadiusRatios.Num() < 2)
		return false;

	if (!FMath::IsNearlyEqual(StageRadiusRatios[0], 1.0f))
		return false;

	if (StageRadiusRatios.Last() != 0.0f)
		return false;

	for (int32 i = 0; i < StageRadiusRatios.Num(); ++i)
	{
		const float Ratio = StageRadiusRatios[i];

		if (!FMath::IsFinite(Ratio) || Ratio < 0.0f || Ratio > 1.0f)
			return false;

		if (i > 0 && Ratio >= StageRadiusRatios[i - 1])
			return false;
	}

	return true;
}
