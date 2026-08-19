// Fill out your copyright notice in the Description page of Project Settings.


#include "Prediction/FJoustInitialPredictionGenerator.h"
#include "Interface/JoustRandomProvider.h"
#include "Prediction/JoustPredictionTypes.h"
#include "Prediction/FJoustPredictionValidator.h"

bool FJoustInitialPredictionGenerator::Generate(
	const FVector2D& TargetPoint,
	float InitialRadius, const 
	FVector2D& LanceBoxMin, const FVector2D& LanceBoxMax, 
	int32 MaxRetries, 
	IJoustRandomProvider& RandomProvider, 
	FJoustPredictionCircle& OutCircle)
{
	OutCircle = FJoustPredictionCircle{};
	
	if (!FMath::IsFinite(TargetPoint.X) ||
		!FMath::IsFinite(TargetPoint.Y) ||
		!FMath::IsFinite(InitialRadius) ||
		!FMath::IsFinite(LanceBoxMin.X) ||
		!FMath::IsFinite(LanceBoxMin.Y) ||
		!FMath::IsFinite(LanceBoxMax.X) ||
		!FMath::IsFinite(LanceBoxMax.Y) ||
		InitialRadius <= 0.0f ||
		MaxRetries <= 0)
		return false;

	FVector2D ValidCenterMin(
		FMath::Max(	LanceBoxMin.X, TargetPoint.X - InitialRadius),
		FMath::Max( LanceBoxMin.Y, TargetPoint.Y - InitialRadius));

	FVector2D ValidCenterMax(
		FMath::Min( LanceBoxMax.X, TargetPoint.X + InitialRadius),
		FMath::Min( LanceBoxMax.Y, TargetPoint.Y + InitialRadius));

	if (ValidCenterMin.X > ValidCenterMax.X ||
		ValidCenterMin.Y > ValidCenterMax.Y)
		return false;

	for (int32 i = 0; i < MaxRetries; ++i)
	{
		FJoustPredictionCircle Candidate;

		Candidate.Center = RandomProvider.GetRandom(ValidCenterMin, ValidCenterMax);

		Candidate.Radius = InitialRadius;

		if (!FJoustPredictionValidator::ValidateInitial(Candidate, TargetPoint, LanceBoxMin, LanceBoxMax))
			continue;

		OutCircle = Candidate;
		return true;
	}

	return false;
}
