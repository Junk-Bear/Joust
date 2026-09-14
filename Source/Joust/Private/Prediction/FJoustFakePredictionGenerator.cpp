// Fill out your copyright notice in the Description page of Project Settings.


#include "Prediction/FJoustFakePredictionGenerator.h"
#include "Interface/JoustRandomProvider.h"
#include "Prediction/FJoustPredictionSeriesGenerator.h"
#include "Prediction/JoustPredictionTypes.h"


bool FJoustFakePredictionGenerator::Generate(
	const FJoustPredictionSettings& InSettings, 
	const FVector2D& InAttackPoint, 
	int32 InPredictionSeed, 
	const FVector2D& InLanceBoxMin, const FVector2D& InLanceBoxMax,
	int32 InMaxRetries, float InMinFakeAttackPointDistance, 
	IJoustRandomProvider& InRandomProvider, 
	TArray<FJoustPredictionSeries>& OutFakeSeries)
{
	OutFakeSeries.Reset();

	if (InSettings.FakeCircleCount < 0 || 
		InMaxRetries <= 0 ||
		!FMath::IsFinite(InMinFakeAttackPointDistance) ||
		InMinFakeAttackPointDistance < 0.0f)
		return false;

	if (InSettings.FakeCircleCount == 0)
		return true;

	TArray<FJoustPredictionSeries> CandidateFakeSeries;

	CandidateFakeSeries.Reserve(InSettings.FakeCircleCount);

	for (int32 i = 0; i < InSettings.FakeCircleCount; ++i)
	{
		FVector2D FakeAttackPoint;

		if (!GenerateFakeAttackPoint(InAttackPoint, InLanceBoxMin, InLanceBoxMax, InMaxRetries, InMinFakeAttackPointDistance, InRandomProvider, FakeAttackPoint))
			return false;

		FJoustPredictionSeries FakeSeries;

		if (!FJoustPredictionSeriesGenerator::Generate(
			InSettings, FakeAttackPoint, InPredictionSeed, InLanceBoxMin, InLanceBoxMax, InMaxRetries, InRandomProvider, FakeSeries))
			return false;

		CandidateFakeSeries.Add(MoveTemp(FakeSeries));
	}
	OutFakeSeries = MoveTemp(CandidateFakeSeries);

	return true;
}

bool FJoustFakePredictionGenerator::GenerateFakeAttackPoint(const FVector2D& InAttackPoint, const FVector2D& InLanceBoxMin, const FVector2D& InLanceBoxMax, int32 InMaxRetries, float InMinFakeAttackPointDistance, IJoustRandomProvider& InRandomProvider, FVector2D& OutFakeAttackPoint)
{
	OutFakeAttackPoint = FVector2D::ZeroVector;

	if (!FMath::IsFinite(InAttackPoint.X) || !FMath::IsFinite(InAttackPoint.Y) ||
		!FMath::IsFinite(InLanceBoxMin.X) || !FMath::IsFinite(InLanceBoxMin.Y) ||
		!FMath::IsFinite(InLanceBoxMax.X) || !FMath::IsFinite(InLanceBoxMax.Y) ||
		!FMath::IsFinite(InMinFakeAttackPointDistance) ||
		InLanceBoxMin.X > InLanceBoxMax.X || InLanceBoxMin.Y > InLanceBoxMax.Y ||
		InMinFakeAttackPointDistance < 0.0f ||
		InMaxRetries <= 0)
		return false;

	if (InAttackPoint.X < InLanceBoxMin.X || InAttackPoint.X > InLanceBoxMax.X ||
		InAttackPoint.Y < InLanceBoxMin.Y || InAttackPoint.Y > InLanceBoxMax.Y)
		return false;

	FVector2D RandomMin = InLanceBoxMin;
	FVector2D RandomMax = InLanceBoxMax;

	for (int32 i = 0; i < InMaxRetries; ++i)
	{
		FVector2D Candidate = InRandomProvider.GetRandom(RandomMin, RandomMax);

		if (!FMath::IsFinite(Candidate.X) || !FMath::IsFinite(Candidate.Y))
			continue;

		if ((Candidate - InAttackPoint).SizeSquared() < FMath::Square(InMinFakeAttackPointDistance))
			continue;

		OutFakeAttackPoint = Candidate;
		return true;
	}

	return false;
}
