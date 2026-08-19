// Fill out your copyright notice in the Description page of Project Settings.


#include "Prediction/FJoustNextPredictionGenerator.h"
#include "Interface/JoustRandomProvider.h"
#include "Prediction/JoustPredictionTypes.h"
#include "Prediction/FJoustPredictionValidator.h"

bool FJoustNextPredictionGenerator::Generate(const FJoustPredictionCircle& CurrentCircle, float NextRadius, const FVector2D& TargetPoint, const FVector2D& LanceBoxMin, const FVector2D& LanceBoxMax, int32 MaxRetries, IJoustRandomProvider& RandomProvider, FJoustPredictionCircle& OutCircle)
{
	OutCircle = FJoustPredictionCircle{};

	if (!FMath::IsFinite(CurrentCircle.Center.X) || !FMath::IsFinite(CurrentCircle.Center.Y) ||
		!FMath::IsFinite(CurrentCircle.Radius) || !FMath::IsFinite(NextRadius) ||
		!FMath::IsFinite(TargetPoint.X) || !FMath::IsFinite(TargetPoint.Y) ||
		!FMath::IsFinite(LanceBoxMin.X) || !FMath::IsFinite(LanceBoxMin.Y) ||
		!FMath::IsFinite(LanceBoxMax.X) || !FMath::IsFinite(LanceBoxMax.Y) ||
		CurrentCircle.Radius <= 0.0f || NextRadius < 0.0f || NextRadius >= CurrentCircle.Radius)
		return false;

	if (LanceBoxMin.X > LanceBoxMax.X ||
		LanceBoxMin.Y > LanceBoxMax.Y)
		return false;

	if (TargetPoint.X < LanceBoxMin.X || TargetPoint.X > LanceBoxMax.X ||
		TargetPoint.Y < LanceBoxMin.Y || TargetPoint.Y > LanceBoxMax.Y)
		return false;

	if (NextRadius == 0.0f)
	{
		FJoustPredictionCircle Candidate;

		Candidate.Center = TargetPoint;
		Candidate.Radius = 0.0f;

		if (!FJoustPredictionValidator::ValidateNext(CurrentCircle, Candidate, TargetPoint, LanceBoxMin, LanceBoxMax))
			return false;

		OutCircle = Candidate;
		return true;
	}

	if (MaxRetries <= 0)
		return false;

	const float CurrentContainmentRadius = CurrentCircle.Radius - NextRadius;

	FVector2D ValidCenterMin(
		FMath::Max3(LanceBoxMin.X, TargetPoint.X - NextRadius, CurrentCircle.Center.X - CurrentContainmentRadius),
		FMath::Max3(LanceBoxMin.Y, TargetPoint.Y - NextRadius, CurrentCircle.Center.Y - CurrentContainmentRadius)
	);

	FVector2D ValidCenterMax(
		FMath::Min3(LanceBoxMax.X, TargetPoint.X + NextRadius, CurrentCircle.Center.X + CurrentContainmentRadius),
		FMath::Min3(LanceBoxMax.Y, TargetPoint.Y + NextRadius, CurrentCircle.Center.Y + CurrentContainmentRadius)
	);

	if (ValidCenterMin.X > ValidCenterMax.X || ValidCenterMin.Y > ValidCenterMax.Y)
		return false;

	for (int32 i = 0; i < MaxRetries; ++i)
	{
		FJoustPredictionCircle Candidate;

		Candidate.Center = RandomProvider.GetRandom(ValidCenterMin, ValidCenterMax);

		Candidate.Radius = NextRadius;

		if (!FJoustPredictionValidator::ValidateNext(CurrentCircle, Candidate, TargetPoint, LanceBoxMin, LanceBoxMax))
			continue;

		OutCircle = Candidate;
		return true;
	}

	return false;
}