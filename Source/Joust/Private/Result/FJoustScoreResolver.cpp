// Fill out your copyright notice in the Description page of Project Settings.


#include "Result/FJoustScoreResolver.h"
#include "Defense/JoustDefenseTypes.h"

int32 FJoustScoreResolver::Resolve(const FJoustDefenseResult& DefenseResult)
{
	return DefenseResult.bBlockedScore ? 0 : 1;
}
