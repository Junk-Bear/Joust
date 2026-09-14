// Fill out your copyright notice in the Description page of Project Settings.


#include "Prediction/FJoustNextPredictionGenerator.h"
#include "Interface/JoustRandomProvider.h"
#include "Prediction/JoustPredictionTypes.h"
#include "Prediction/FJoustPredictionValidator.h"

bool FJoustNextPredictionGenerator::Generate(const FJoustPredictionCircle& InCurrentCircle, float InNextRadius, const FVector2D& InTargetPoint, const FVector2D& InLanceBoxMin, const FVector2D& InLanceBoxMax, int32 InMaxRetries, IJoustRandomProvider& InRandomProvider, FJoustPredictionCircle& OutCircle)
{
	OutCircle = FJoustPredictionCircle{};

	if (!FMath::IsFinite(InCurrentCircle.Center.X) || !FMath::IsFinite(InCurrentCircle.Center.Y) ||
		!FMath::IsFinite(InCurrentCircle.Radius) || !FMath::IsFinite(InNextRadius) ||
		!FMath::IsFinite(InTargetPoint.X) || !FMath::IsFinite(InTargetPoint.Y) ||
		!FMath::IsFinite(InLanceBoxMin.X) || !FMath::IsFinite(InLanceBoxMin.Y) ||
		!FMath::IsFinite(InLanceBoxMax.X) || !FMath::IsFinite(InLanceBoxMax.Y) ||
		InCurrentCircle.Radius <= 0.0f || InNextRadius < 0.0f || InNextRadius >= InCurrentCircle.Radius)
		return false;

	if (InLanceBoxMin.X > InLanceBoxMax.X ||
		InLanceBoxMin.Y > InLanceBoxMax.Y)
		return false;

	if (InTargetPoint.X < InLanceBoxMin.X || InTargetPoint.X > InLanceBoxMax.X ||
		InTargetPoint.Y < InLanceBoxMin.Y || InTargetPoint.Y > InLanceBoxMax.Y)
		return false;

	if (InNextRadius == 0.0f)
	{
		FJoustPredictionCircle Candidate;

		Candidate.Center = InTargetPoint;
		Candidate.Radius = 0.0f;

		if (!FJoustPredictionValidator::ValidateNext(InCurrentCircle, Candidate, InTargetPoint, InLanceBoxMin, InLanceBoxMax))
			return false;

		OutCircle = Candidate;
		return true;
	}

	if (InMaxRetries <= 0)
		return false;

	const float CurrentContainmentRadius = InCurrentCircle.Radius - InNextRadius;

	FVector2D ValidCenterMin(
		FMath::Max3(InLanceBoxMin.X, InTargetPoint.X - InNextRadius, InCurrentCircle.Center.X - CurrentContainmentRadius),
		FMath::Max3(InLanceBoxMin.Y, InTargetPoint.Y - InNextRadius, InCurrentCircle.Center.Y - CurrentContainmentRadius)
	);

	FVector2D ValidCenterMax(
		FMath::Min3(InLanceBoxMax.X, InTargetPoint.X + InNextRadius, InCurrentCircle.Center.X + CurrentContainmentRadius),
		FMath::Min3(InLanceBoxMax.Y, InTargetPoint.Y + InNextRadius, InCurrentCircle.Center.Y + CurrentContainmentRadius)
	);

	if (ValidCenterMin.X > ValidCenterMax.X || ValidCenterMin.Y > ValidCenterMax.Y)
		return false;

	for (int32 i = 0; i < InMaxRetries; ++i)
	{
		FJoustPredictionCircle Candidate;

		Candidate.Center = InRandomProvider.GetRandom(ValidCenterMin, ValidCenterMax);

		Candidate.Radius = InNextRadius;

		if (!FJoustPredictionValidator::ValidateNext(InCurrentCircle, Candidate, InTargetPoint, InLanceBoxMin, InLanceBoxMax))
			continue;

		OutCircle = Candidate;
		return true;
	}

	return false;
}