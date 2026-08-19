// Fill out your copyright notice in the Description page of Project Settings.


#include "Prediction/JoustPredictionService.h"
#include "Attack/JoustAttackTypeDataAsset.h"
#include "Interface/JoustRandomProvider.h"
#include "Prediction/FJoustFakePredictionGenerator.h"
#include "Prediction/FJoustPredictionSeriesGenerator.h"
#include "Prediction/FJoustPredictionSettingsResolver.h"
#include "Prediction/FJoustPredictionTypeResolver.h"
#include "Rules/JoustRuleSetDataAsset.h"

void UJoustPredictionService::Initialize(UJoustRuleSetDataAsset* InRuleSet, IJoustRandomProvider* InRandomProvider)
{
	RuleSet = InRuleSet;
	RandomProvider = InRandomProvider;

	ResetPredictionData();
}

bool UJoustPredictionService::PreparePrediction(
	EJoustAttackType AttackType, const FVector2D & AttackPoint, 
	int32 PredictionSeed, 
	float AttackerDeception, float AttackerQuickness, 
	float DefenderReading)
{
	ResetPredictionData();

	if (!RuleSet || !RandomProvider)
		return false;

	const UJoustAttackTypeDataAsset* AttackTypeData = FJoustPredictionTypeResolver::Resolve(*RuleSet, AttackType);

	if (!AttackTypeData)
		return false;

	FJoustPredictionSettings CandidateSettings;

	if (!FJoustPredictionSettingsResolver::Resolve(
		*RuleSet, *AttackTypeData,
		AttackerDeception, AttackerQuickness,
		DefenderReading,
		CandidateSettings))
		return false;
		
	FJoustPredictionSeries CandidateRealSeries;

	if (!FJoustPredictionSeriesGenerator::Generate(
		CandidateSettings,
		AttackPoint,
		PredictionSeed,
		RuleSet->LanceBoxMin,
		RuleSet->LanceBoxMax,
		RuleSet->MaxPredictionAttempts,
		*RandomProvider,
		CandidateRealSeries))
		return false;

	TArray<FJoustPredictionSeries> CandidateFakeSeries;

	if (!FJoustFakePredictionGenerator::Generate(
		CandidateSettings,
		AttackPoint,
		PredictionSeed,
		RuleSet->LanceBoxMin,
		RuleSet->LanceBoxMax,
		RuleSet->MaxPredictionAttempts,
		RuleSet->MinFakeAttackPointDistance,
		*RandomProvider,
		CandidateFakeSeries))
		return false;

	CurrentSettings = MoveTemp(CandidateSettings);

	RealSeries = MoveTemp(CandidateRealSeries);

	FakeSeries = MoveTemp(CandidateFakeSeries);

	bPrepared = true;

	return true;
}

void UJoustPredictionService::EndRound()
{
	ResetPredictionData();
}

void UJoustPredictionService::ResetPredictionData()
{
	CurrentSettings = FJoustPredictionSettings{};

	RealSeries = FJoustPredictionSeries{};

	FakeSeries.Reset();

	bPrepared = false;
}
