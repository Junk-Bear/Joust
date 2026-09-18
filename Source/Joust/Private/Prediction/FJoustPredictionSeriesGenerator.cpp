// Fill out your copyright notice in the Description page of Project Settings.


#include "Prediction/FJoustPredictionSeriesGenerator.h"
#include "Prediction/FJoustInitialPredictionGenerator.h"
#include "Prediction/FJoustNextPredictionGenerator.h"
#include "Prediction/JoustPredictionTypes.h"

bool FJoustPredictionSeriesGenerator::Generate(
	const FJoustPredictionSettings& InSettings, 
	const FVector2D& InTargetPoint, 
	int32 InPredictionSeed, 
	const FVector2D& InLanceBoxMin, 
	const FVector2D& InLanceBoxMax, 
	int32 InMaxRetries, 
	IJoustRandomProvider& InRandomProvider, 
	FJoustPredictionSeries& OutSeries)
{
	OutSeries = FJoustPredictionSeries{};

	if (!FMath::IsFinite(InSettings.InitialRadius) ||
		InSettings.InitialRadius <= 0.0f ||
		InSettings.StageCount < 2 ||
		InSettings.StageRadiusRatios.Num() != InSettings.StageCount ||
		InSettings.StageDurations.Num() != InSettings.StageCount - 1 ||
		InMaxRetries <= 0)
		return false;

	FJoustPredictionSeries CandidateSeries;

	CandidateSeries.Circles.Reserve(
		InSettings.StageCount);

	FJoustPredictionCircle CurrentCircle;

	if (!FJoustInitialPredictionGenerator::Generate(
		InTargetPoint,
		InSettings.InitialRadius,
		InLanceBoxMin,
		InLanceBoxMax,
		InMaxRetries,
		InRandomProvider,
		CurrentCircle))
		return false;

	CandidateSeries.Circles.Add(
		CurrentCircle);

	for (int32 i = 1; i < InSettings.StageCount; ++i)
	{
		FJoustPredictionCircle NextCircle;

		if (!FJoustNextPredictionGenerator::Generate(
			CurrentCircle,
			InSettings.InitialRadius *
			InSettings.StageRadiusRatios[i],
			InTargetPoint,
			InLanceBoxMin,
			InLanceBoxMax,
			InMaxRetries,
			InRandomProvider,
			NextCircle))
			return false;

		CandidateSeries.Circles.Add(NextCircle);

		CurrentCircle = NextCircle;
	}

	CandidateSeries.PredictionSeed = InPredictionSeed;

	OutSeries = MoveTemp(CandidateSeries);

	return true;
}
