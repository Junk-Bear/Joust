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
	EJoustAttackType InAttackType, const FVector2D & InAttackPoint, 
	int32 InPredictionSeed, 
	float InAttackerDeception, float InAttackerQuickness, 
	float InDefenderReading)
{
	ResetPredictionData();

	if (!IsValid(RuleSet) || !RandomProvider)
		return false;

	const UJoustAttackTypeDataAsset* AttackTypeDataPtr = FJoustPredictionTypeResolver::Resolve(*RuleSet, InAttackType);
	if (!IsValid(AttackTypeDataPtr))
		return false;

	FJoustPredictionSettings CandidateSettings;

	if (!FJoustPredictionSettingsResolver::Resolve(
		*RuleSet, *AttackTypeDataPtr,
		InAttackerDeception, InAttackerQuickness,
		InDefenderReading,
		CandidateSettings))
		return false;
		
	FJoustPredictionSeries CandidateRealSeries;

	if (!FJoustPredictionSeriesGenerator::Generate(
		CandidateSettings,
		InAttackPoint,
		InPredictionSeed,
		RuleSet->LanceBoxMin,
		RuleSet->LanceBoxMax,
		RuleSet->MaxPredictionAttempts,
		*RandomProvider,
		CandidateRealSeries))
		return false;

	TArray<FJoustPredictionSeries> CandidateFakeSeries;

	if (!FJoustFakePredictionGenerator::Generate(
		CandidateSettings,
		InAttackPoint,
		InPredictionSeed,
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
