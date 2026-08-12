// Fill out your copyright notice in the Description page of Project Settings.


#include "Random/JoustDefaultRandomProvider.h"

void UJoustDefaultRandomProvider::Initialize(int32 InSeed)
{
	RandomStream.Initialize(InSeed);
}

int32 UJoustDefaultRandomProvider::GetRandom(int32 Min, int32 Max)
{
	return RandomStream.RandRange(Min, Max);
}

float UJoustDefaultRandomProvider::GetRandom(float Min, float Max)
{
	return RandomStream.FRandRange(Min, Max);
}

FVector2D UJoustDefaultRandomProvider::GetRandom(FVector2D& Min, FVector2D& Max)
{
	return FVector2D(
		RandomStream.FRandRange(Min.X, Max.X), 
		RandomStream.FRandRange(Min.Y, Max.Y)
	);
}