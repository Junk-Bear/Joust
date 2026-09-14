// Fill out your copyright notice in the Description page of Project Settings.


#include "Prediction/JoustPredictionInterpolation.h"
#include "Prediction/JoustPredictionTypes.h"

bool UJoustPredictionInterpolation::Interpolate(
	const FJoustPredictionCircle& InCurrentCircle, 
	const FJoustPredictionCircle& InTargetCircle, 
	float InElapsedTime, 
	float InStageDuration, 
	FJoustPredictionDisplayCircle& OutDisplayCircle, 
	float& OutAlpha) const
{
	if (!FMath::IsFinite(InCurrentCircle.Center.X) || !FMath::IsFinite(InCurrentCircle.Center.Y) ||
		!FMath::IsFinite(InCurrentCircle.Radius) ||
		!FMath::IsFinite(InTargetCircle.Center.X) || !FMath::IsFinite(InTargetCircle.Center.Y) ||
		!FMath::IsFinite(InTargetCircle.Radius) ||
		!FMath::IsFinite(InElapsedTime) ||
		!FMath::IsFinite(InStageDuration) ||
		InCurrentCircle.Radius < 0.0f || InTargetCircle.Radius < 0.0f ||
		InElapsedTime < 0.0f ||
		InStageDuration <= 0.0f)
		return false;

	OutAlpha = FMath::Clamp(InElapsedTime / InStageDuration, 0.0f, 1.0f);

	OutDisplayCircle.Center = FMath::Lerp(InCurrentCircle.Center, InTargetCircle.Center, OutAlpha);

	OutDisplayCircle.Radius = FMath::Lerp(InCurrentCircle.Radius, InTargetCircle.Radius, OutAlpha);

	return true;
}
