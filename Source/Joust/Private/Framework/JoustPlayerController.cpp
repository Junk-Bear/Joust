// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/JoustPlayerController.h"

void AJoustPlayerController::SetSelectedStrategyCardID(FName InCardID)
{
	SelectedStrategyCardID = InCardID;
	bHasSelectedStrategyCardID = !InCardID.IsNone();
}

void AJoustPlayerController::SetBannedStrategyCardID(FName InCardID)
{
	BannedStrategyCardID = InCardID;
	bHasBannedStrategyCardID = !InCardID.IsNone();
}

void AJoustPlayerController::ResetStrategyInput()
{
	SelectedStrategyCardID = NAME_None;
	BannedStrategyCardID = NAME_None;

	bHasSelectedStrategyCardID = false;
	bHasBannedStrategyCardID = false;
}

bool AJoustPlayerController::TryGetSelectedStrategyCardID(FName & OutCardID) const
{
	if (!bHasSelectedStrategyCardID)
	{
		OutCardID = NAME_None;

		return false;
	}

	OutCardID = SelectedStrategyCardID;

	return true;
}

bool AJoustPlayerController::TryGetBannedStrategyCardID(FName& OutCardID) const
{
	if (!bHasBannedStrategyCardID)
	{
		OutCardID = NAME_None;

		return false;
	}

	OutCardID = BannedStrategyCardID;

	return true;
}

void AJoustPlayerController::SetAttackPoint(const FVector2D& InAttackPoint)
{
	AttackPoint = InAttackPoint;
}

void AJoustPlayerController::SetAttackType(EJoustAttackType InAttackType)
{
	AttackType = InAttackType;
}

void AJoustPlayerController::ConfirmAttack()
{
	bAttackConfirmed = true;
}

void AJoustPlayerController::ResetAttackInput()
{
	AttackPoint = FVector2D::ZeroVector;

	AttackType = EJoustAttackType::Normal;

	bAttackConfirmed = false;
}

FVector2D AJoustPlayerController::GetAttackPoint() const
{
	return AttackPoint;
}

EJoustAttackType AJoustPlayerController::GetAttackType() const
{
	return AttackType;
}

bool AJoustPlayerController::IsAttackConfirmed() const
{
	return bAttackConfirmed;
}

void AJoustPlayerController::SetShieldPoint(const FVector2D& InShieldPoint)
{
	ShieldPoint = InShieldPoint;
}

void AJoustPlayerController::SetParryAttempt(float InParryInputTime)
{
	bParryAttempted = true;

	ParryInputTime = InParryInputTime;
}

void AJoustPlayerController::ResetDefenseInput()
{
	ShieldPoint = FVector2D::ZeroVector;

	bParryAttempted = false;

	ParryInputTime = 0.0f;
}

FVector2D AJoustPlayerController::GetShieldPoint() const
{
	return ShieldPoint;
}

bool AJoustPlayerController::IsParryAttempted() const
{
	return bParryAttempted;
}

float AJoustPlayerController::GetParryInputTime() const
{
	return ParryInputTime;
}
