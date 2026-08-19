// Fill out your copyright notice in the Description page of Project Settings.


#include "Prediction/FJoustFakePredictionGenerator.h"
#include "Interface/JoustRandomProvider.h"
#include "Prediction/FJoustPredictionSeriesGenerator.h"
#include "Prediction/JoustPredictionTypes.h"


bool FJoustFakePredictionGenerator::Generate(
	const FJoustPredictionSettings& Settings, 
	const FVector2D& AttackPoint, 
	int32 PredictionSeed, 
	const FVector2D& LanceBoxMin, const FVector2D& LanceBoxMax,
	int32 MaxRetries, float MinFakeAttackPointDistance, 
	IJoustRandomProvider& RandomProvider, 
	TArray<FJoustPredictionSeries>& OutFakeSeries)
{
	OutFakeSeries.Reset();

	if (Settings.FakeCircleCount < 0 || 
		MaxRetries <= 0 ||
		!FMath::IsFinite(MinFakeAttackPointDistance) ||
		MinFakeAttackPointDistance < 0.0f)
		return false;

	if (Settings.FakeCircleCount == 0)
		return true;

	TArray<FJoustPredictionSeries> CandidateFakeSeries;

	CandidateFakeSeries.Reserve(Settings.FakeCircleCount);

	for (int32 i = 0; i < Settings.FakeCircleCount; ++i)
	{
		FVector2D FakeAttackPoint;

		if (!GenerateFakeAttackPoint(AttackPoint, LanceBoxMin, LanceBoxMax, MaxRetries, MinFakeAttackPointDistance, RandomProvider, FakeAttackPoint))
			return false;

		FJoustPredictionSeries FakeSeries;

		if (!FJoustPredictionSeriesGenerator::Generate(
			Settings, FakeAttackPoint, PredictionSeed, LanceBoxMin, LanceBoxMax, MaxRetries, RandomProvider, FakeSeries))
			return false;

		CandidateFakeSeries.Add(MoveTemp(FakeSeries));
	}
	OutFakeSeries = MoveTemp(CandidateFakeSeries);

	return true;
}

bool FJoustFakePredictionGenerator::GenerateFakeAttackPoint(const FVector2D& AttackPoint, const FVector2D& LanceBoxMin, const FVector2D& LanceBoxMax, int32 MaxRetries, float MinFakeAttackPointDistance, IJoustRandomProvider& RandomProvider, FVector2D& OutFakeAttackPoint)
{
	OutFakeAttackPoint = FVector2D::ZeroVector;

	if (!FMath::IsFinite(AttackPoint.X) || !FMath::IsFinite(AttackPoint.Y) ||
		!FMath::IsFinite(LanceBoxMin.X) || !FMath::IsFinite(LanceBoxMin.Y) ||
		!FMath::IsFinite(LanceBoxMax.X) || !FMath::IsFinite(LanceBoxMax.Y) ||
		!FMath::IsFinite(MinFakeAttackPointDistance) ||
		LanceBoxMin.X > LanceBoxMax.X || LanceBoxMin.Y > LanceBoxMax.Y ||
		MinFakeAttackPointDistance < 0.0f ||
		MaxRetries <= 0)
		return false;

	if (AttackPoint.X < LanceBoxMin.X || AttackPoint.X > LanceBoxMax.X ||
		AttackPoint.Y < LanceBoxMin.Y || AttackPoint.Y > LanceBoxMax.Y)
		return false;

	FVector2D RandomMin = LanceBoxMin;
	FVector2D RandomMax = LanceBoxMax;

	for (int32 i = 0; i < MaxRetries; ++i)
	{
		FVector2D Candidate = RandomProvider.GetRandom(RandomMin, RandomMax);

		if (!FMath::IsFinite(Candidate.X) || !FMath::IsFinite(Candidate.Y))
			continue;

		if ((Candidate - AttackPoint).SizeSquared() < FMath::Square(MinFakeAttackPointDistance))
			continue;

		OutFakeAttackPoint = Candidate;
		return true;
	}

	return false;
}
