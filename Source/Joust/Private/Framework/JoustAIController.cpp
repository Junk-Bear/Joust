// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/JoustAIController.h"

AJoustAIController::AJoustAIController()
{
	//AI도 플레이어스테이트가 필요함.
	bWantsPlayerState = true;
}

void AJoustAIController::SetSelectedStrategyCardID(FName InCardID)
{
	SelectedStrategyCardID = InCardID;
	bHasSelectedStrategyCardID = !InCardID.IsNone();
}

void AJoustAIController::SetBannedStrategyCardID(FName InCardID)
{
	BannedStrategyCardID = InCardID;
	bHasBannedStrategyCardID = !InCardID.IsNone();
}

void AJoustAIController::ResetStrategyInput()
{
	SelectedStrategyCardID = NAME_None;
	BannedStrategyCardID = NAME_None;

	bHasSelectedStrategyCardID = false;
	bHasBannedStrategyCardID = false;
}

bool AJoustAIController::TryGetSelectedStrategyCardID(FName& OutCardID) const
{
	if (!bHasSelectedStrategyCardID)
	{
		OutCardID = NAME_None;

		return false;
	}

	OutCardID = SelectedStrategyCardID;

	return true;
}

bool AJoustAIController::TryGetBannedStrategyCardID(FName& OutCardID) const
{
	if (!bHasBannedStrategyCardID)
	{
		OutCardID = NAME_None;

		return false;
	}

	OutCardID = BannedStrategyCardID;

	return true;
}

void AJoustAIController::SetAttackPoint(const FVector2D& InAttackPoint)
{
	AttackPoint = InAttackPoint;
}

void AJoustAIController::SetAttackType(EJoustAttackType InAttackType)
{
	AttackType = InAttackType;
}

void AJoustAIController::ConfirmAttack()
{
	bAttackConfirmed = true;
}

void AJoustAIController::ResetAttackInput()
{
	AttackPoint = FVector2D::ZeroVector;

	AttackType = EJoustAttackType::Normal;

	bAttackConfirmed = false;
}

FVector2D AJoustAIController::GetAttackPoint() const
{
	return AttackPoint;
}

EJoustAttackType AJoustAIController::GetAttackType() const
{
	return AttackType;
}

bool AJoustAIController::IsAttackConfirmed() const
{
	return bAttackConfirmed;
}

void AJoustAIController::SetShieldPoint(const FVector2D& InShieldPoint)
{
	ShieldPoint = InShieldPoint;
}

void AJoustAIController::SetParryAttempt(float InParryInputTime)
{
	bParryAttempted = true;

	ParryInputTime = InParryInputTime;
}

void AJoustAIController::ResetDefenseInput()
{
	ShieldPoint = FVector2D::ZeroVector;

	bParryAttempted = false;

	ParryInputTime = 0.0f;
}

FVector2D AJoustAIController::GetShieldPoint() const
{
	return ShieldPoint;
}

bool AJoustAIController::IsParryAttempted() const
{
	return bParryAttempted;
}

float AJoustAIController::GetParryInputTime() const
{
	return ParryInputTime;
}