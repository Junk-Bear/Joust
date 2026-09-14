// Fill out your copyright notice in the Description page of Project Settings.


#include "Attack/FJoustAttackValidator.h"
#include "Attack/FJoustAttackUsageTracker.h"
#include "Attack/JoustAttackTypeDataAsset.h"
#include "Rules/JoustRuleSetDataAsset.h"

bool FJoustAttackValidator::Validate(const UJoustRuleSetDataAsset& InRuleSet, const FJoustAttackUsageTracker& InUsageTracker, const FVector2D& InAttackPoint, EJoustAttackType InAttackType)
{
	if (!FMath::IsFinite(InAttackPoint.X) || !FMath::IsFinite(InAttackPoint.Y))
		return false;

	if ((InRuleSet.LanceBoxMin.X > InRuleSet.LanceBoxMax.X) || (InRuleSet.LanceBoxMin.Y > InRuleSet.LanceBoxMax.Y))
		return false;

	if (InAttackPoint.X < InRuleSet.LanceBoxMin.X ||
		InAttackPoint.X > InRuleSet.LanceBoxMax.X ||
		InAttackPoint.Y < InRuleSet.LanceBoxMin.Y ||
		InAttackPoint.Y > InRuleSet.LanceBoxMax.Y)
		return false;

	const TObjectPtr<UJoustAttackTypeDataAsset>* AttackTypeSettingPtr = InRuleSet.AttackTypeSettings.Find(InAttackType);

	if (AttackTypeSettingPtr == nullptr)
		return false;

	const UJoustAttackTypeDataAsset* AttackTypeDataPtr = AttackTypeSettingPtr->Get();

	if (!IsValid(AttackTypeDataPtr))
		return false;

	if (!InUsageTracker.CanUse(InAttackType))
		return false;

	return true;
}
