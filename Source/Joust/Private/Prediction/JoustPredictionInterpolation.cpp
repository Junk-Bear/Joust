// Fill out your copyright notice in the Description page of Project Settings.


#include "Prediction/JoustPredictionInterpolation.h"
#include "Prediction/JoustPredictionTypes.h"

bool UJoustPredictionInterpolation::Interpolate(
	const FJoustPredictionCircle& CurrentCircle, 
	const FJoustPredictionCircle& TargetCircle, 
	float ElapsedTime, 
	float StageDuration, 
	FJoustPredictionDisplayCircle& OutDisplayCircle, 
	float& OutAlpha) const
{
	if (!FMath::IsFinite(CurrentCircle.Center.X) || !FMath::IsFinite(CurrentCircle.Center.Y) ||
		!FMath::IsFinite(CurrentCircle.Radius) ||
		!FMath::IsFinite(TargetCircle.Center.X) || !FMath::IsFinite(TargetCircle.Center.Y) ||
		!FMath::IsFinite(TargetCircle.Radius) ||
		!FMath::IsFinite(ElapsedTime) ||
		!FMath::IsFinite(StageDuration) ||
		CurrentCircle.Radius < 0.0f || TargetCircle.Radius < 0.0f ||
		ElapsedTime < 0.0f ||
		StageDuration <= 0.0f)
		return false;

	OutAlpha = FMath::Clamp(ElapsedTime / StageDuration, 0.0f, 1.0f);

	OutDisplayCircle.Center = FMath::Lerp(CurrentCircle.Center, TargetCircle.Center, OutAlpha);

	OutDisplayCircle.Radius = FMath::Lerp(CurrentCircle.Radius, TargetCircle.Radius, OutAlpha);

	return true;
}
