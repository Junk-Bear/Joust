// Fill out your copyright notice in the Description page of Project Settings.


#include "Random/JoustDefaultRandomProvider.h"

void UJoustDefaultRandomProvider::Initialize(int32 InSeed)
{
	RandomStream.Initialize(InSeed);
}

int32 UJoustDefaultRandomProvider::GetRandom(int32 InMin, int32 InMax)
{
	return RandomStream.RandRange(InMin, InMax);
}

float UJoustDefaultRandomProvider::GetRandom(float InMin, float InMax)
{
	return RandomStream.FRandRange(InMin, InMax);
}

FVector2D UJoustDefaultRandomProvider::GetRandom(const FVector2D& InMin, const FVector2D& InMax)
{
	return FVector2D(RandomStream.FRandRange(InMin.X, InMax.X), RandomStream.FRandRange(InMin.Y, InMax.Y));
}