// Fill out your copyright notice in the Description page of Project Settings.


#include "Attack/FJoustAttackValidator.h"
#include "Attack/FJoustAttackUsageTracker.h"
#include "Attack/JoustAttackTypeDataAsset.h"
#include "Rules/JoustRuleSetDataAsset.h"

bool FJoustAttackValidator::Validate(const UJoustRuleSetDataAsset& RuleSet, const FJoustAttackUsageTracker& UsageTracker, const FVector2D& AttackPoint, EJoustAttackType AttackType)
{
	if (!FMath::IsFinite(AttackPoint.X) || !FMath::IsFinite(AttackPoint.Y))
		return false;

	if ((RuleSet.LanceBoxMin.X > RuleSet.LanceBoxMax.X) || (RuleSet.LanceBoxMin.Y > RuleSet.LanceBoxMax.Y))
		return false;

	if (AttackPoint.X < RuleSet.LanceBoxMin.X ||
		AttackPoint.X > RuleSet.LanceBoxMax.X ||
		AttackPoint.Y < RuleSet.LanceBoxMin.Y ||
		AttackPoint.Y > RuleSet.LanceBoxMax.Y)
		return false;

	const TObjectPtr<UJoustAttackTypeDataAsset>* AttackTypeDataPtr = RuleSet.AttackTypeSettings.Find(AttackType);

	if (AttackTypeDataPtr == nullptr)
		return false;

	const UJoustAttackTypeDataAsset* AttackTypeData = AttackTypeDataPtr->Get();

	if (AttackTypeData == nullptr)
		return false;

	if (!UsageTracker.CanUse(AttackType))
		return false;

	return true;
}
