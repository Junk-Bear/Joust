// Fill out your copyright notice in the Description page of Project Settings.


#include "Prediction/FJoustInitialPredictionGenerator.h"
#include "Interface/JoustRandomProvider.h"
#include "Prediction/JoustPredictionTypes.h"
#include "Prediction/FJoustPredictionValidator.h"

bool FJoustInitialPredictionGenerator::Generate(
	const FVector2D& InTargetPoint,
	float InInitialRadius, const 
	FVector2D& InLanceBoxMin, const FVector2D& InLanceBoxMax, 
	int32 InMaxRetries, 
	IJoustRandomProvider& InRandomProvider, 
	FJoustPredictionCircle& OutCircle)
{
	OutCircle = FJoustPredictionCircle{};
	
	if (!FMath::IsFinite(InTargetPoint.X) ||
		!FMath::IsFinite(InTargetPoint.Y) ||
		!FMath::IsFinite(InInitialRadius) ||
		!FMath::IsFinite(InLanceBoxMin.X) ||
		!FMath::IsFinite(InLanceBoxMin.Y) ||
		!FMath::IsFinite(InLanceBoxMax.X) ||
		!FMath::IsFinite(InLanceBoxMax.Y) ||
		InInitialRadius <= 0.0f ||
		InMaxRetries <= 0)
		return false;

	FVector2D ValidCenterMin(
		FMath::Max(	InLanceBoxMin.X, InTargetPoint.X - InInitialRadius),
		FMath::Max( InLanceBoxMin.Y, InTargetPoint.Y - InInitialRadius));

	FVector2D ValidCenterMax(
		FMath::Min( InLanceBoxMax.X, InTargetPoint.X + InInitialRadius),
		FMath::Min( InLanceBoxMax.Y, InTargetPoint.Y + InInitialRadius));

	if (ValidCenterMin.X > ValidCenterMax.X ||
		ValidCenterMin.Y > ValidCenterMax.Y)
		return false;

	for (int32 i = 0; i < InMaxRetries; ++i)
	{
		FJoustPredictionCircle Candidate;

		Candidate.Center = InRandomProvider.GetRandom(ValidCenterMin, ValidCenterMax);

		Candidate.Radius = InInitialRadius;

		if (!FJoustPredictionValidator::ValidateInitial(Candidate, InTargetPoint, InLanceBoxMin, InLanceBoxMax))
			continue;

		OutCircle = Candidate;
		return true;
	}

	return false;
}
