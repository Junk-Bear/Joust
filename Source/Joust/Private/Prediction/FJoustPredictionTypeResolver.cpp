// Fill out your copyright notice in the Description page of Project Settings.


#include "Prediction/FJoustPredictionTypeResolver.h"
#include "Attack/JoustAttackTypeDataAsset.h"
#include "Rules/JoustRuleSetDataAsset.h"

const UJoustAttackTypeDataAsset* FJoustPredictionTypeResolver::Resolve(const UJoustRuleSetDataAsset& InRuleSet, EJoustAttackType InAttackType)
{
	return InRuleSet.AttackTypeSettings.FindRef(InAttackType).Get();
}
