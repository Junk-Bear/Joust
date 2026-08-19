// Fill out your copyright notice in the Description page of Project Settings.


#include "Prediction/FJoustPredictionSeriesGenerator.h"
#include "Prediction/FJoustInitialPredictionGenerator.h"
#include "Prediction/FJoustNextPredictionGenerator.h"
#include "Prediction/JoustPredictionTypes.h"

bool FJoustPredictionSeriesGenerator::Generate(
	const FJoustPredictionSettings& Settings, 
	const FVector2D& TargetPoint, 
	int32 PredictionSeed, 
	const FVector2D& LanceBoxMin, 
	const FVector2D& LanceBoxMax, 
	int32 MaxRetries, 
	IJoustRandomProvider& RandomProvider, 
	FJoustPredictionSeries& OutSeries)
{
	OutSeries = FJoustPredictionSeries{};

	if (!FMath::IsFinite(Settings.InitialRadius) ||
		Settings.InitialRadius <= 0.0f ||
		Settings.StageCount < 2 ||
		Settings.StageRadiusRatios.Num() != Settings.StageCount ||
		Settings.StageDurations.Num() != Settings.StageCount - 1 ||
		MaxRetries <= 0)
		return false;

	FJoustPredictionSeries CandidateSeries;

	CandidateSeries.Circles.Reserve(
		Settings.StageCount);

	FJoustPredictionCircle CurrentCircle;

	if (!FJoustInitialPredictionGenerator::Generate(
		TargetPoint,
		Settings.InitialRadius,
		LanceBoxMin,
		LanceBoxMax,
		MaxRetries,
		RandomProvider,
		CurrentCircle))
		return false;

	CandidateSeries.Circles.Add(
		CurrentCircle);

	for (int i = 0; i < Settings.StageCount; ++i)
	{
		FJoustPredictionCircle NextCircle;

		if (!FJoustNextPredictionGenerator::Generate(
			CurrentCircle,
			Settings.InitialRadius *
			Settings.StageRadiusRatios[i],
			TargetPoint,
			LanceBoxMin,
			LanceBoxMax,
			MaxRetries,
			RandomProvider,
			NextCircle))
			return false;

		CandidateSeries.Circles.Add(NextCircle);

		CurrentCircle = NextCircle;
	}

	CandidateSeries.PredictionSeed = PredictionSeed;

	OutSeries = MoveTemp(CandidateSeries);

	return true;
}
