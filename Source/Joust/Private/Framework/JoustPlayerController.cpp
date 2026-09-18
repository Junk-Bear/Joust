// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/JoustPlayerController.h"
#include "Engine/World.h"
#include "Framework/JoustGameMode.h"

void AJoustPlayerController::RequestStartMatch()
{
	if (HasAuthority())
	{
		HandleStartMatchRequest();

		return;
	}

	ServerRequestStartMatch();
}

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

void AJoustPlayerController::RequestStrategySelection(FName InCardID)
{
	if (!IsLocalController() || InCardID.IsNone())
		return;

	if (HasAuthority())
	{
		HandleStrategySelectionRequest(InCardID);

		return;
	}

	ServerRequestStrategySelection(InCardID);
}

void AJoustPlayerController::RequestStrategyBan(FName InCardID)
{
	if (!IsLocalController() || InCardID.IsNone())
		return;

	if (HasAuthority())
	{
		HandleStrategyBanRequest(InCardID);

		return;
	}

	ServerRequestStrategyBan(InCardID);
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

void AJoustPlayerController::RequestAttack(EJoustAttackType InAttackType, const FVector2D& InAttackPoint)
{
	if (!IsLocalController() || !FMath::IsFinite(InAttackPoint.X) || !FMath::IsFinite(InAttackPoint.Y))
		return;

	if (HasAuthority())
	{
		HandleAttackRequest(InAttackType, InAttackPoint);

		return;
	}

	ServerRequestAttack(InAttackType, InAttackPoint);
}

void AJoustPlayerController::RequestShieldPoint(const FVector2D& InShieldPoint)
{
	if (!IsLocalController() || !FMath::IsFinite(InShieldPoint.X) || !FMath::IsFinite(InShieldPoint.Y))
		return;

	SetShieldPoint(InShieldPoint);

	if (HasAuthority())
	{
		HandleDefenseRequest(InShieldPoint, false, 0.0f);

		return;
	}

	ServerRequestShieldPoint(InShieldPoint);
}


void AJoustPlayerController::RequestParry(const FVector2D & InShieldPoint, float InParryInputTime)
{
	if (!IsLocalController() || bParryAttempted || !FMath::IsFinite(InShieldPoint.X) || !FMath::IsFinite(InShieldPoint.Y) || !FMath::IsFinite(InParryInputTime))
		return;

	SetShieldPoint(InShieldPoint);
	SetParryAttempt(InParryInputTime);

	if (HasAuthority())
	{
		HandleDefenseRequest(InShieldPoint, true, InParryInputTime);

		return;
	}

	ServerRequestParry(InShieldPoint, InParryInputTime);
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

void AJoustPlayerController::ServerRequestStrategyBan_Implementation(FName InCardID)
{
	HandleStrategyBanRequest(InCardID);
}

void AJoustPlayerController::HandleStrategySelectionRequest(FName InCardID)
{
	if (!HasAuthority() || InCardID.IsNone())
		return;

	UWorld* WorldPtr = GetWorld();
	if (!IsValid(WorldPtr))
		return;

	AJoustGameMode* GameModePtr = WorldPtr->GetAuthGameMode<AJoustGameMode>();
	if (!IsValid(GameModePtr))
		return;

	GameModePtr->SubmitPlayerStrategySelection(this, InCardID);
}

void AJoustPlayerController::ServerRequestStrategySelection_Implementation(FName InCardID)
{
	HandleStrategySelectionRequest(InCardID);
}

void AJoustPlayerController::HandleStrategyBanRequest(FName InCardID)
{
	if (!HasAuthority() || InCardID.IsNone())
		return;

	UWorld* WorldPtr = GetWorld();
	if (!IsValid(WorldPtr))
		return;

	AJoustGameMode* GameModePtr = WorldPtr->GetAuthGameMode<AJoustGameMode>();
	if (!IsValid(GameModePtr))
		return;

	GameModePtr->SubmitPlayerStrategyBan(
		this,
		InCardID);
}

void AJoustPlayerController::HandleAttackRequest(EJoustAttackType InAttackType, const FVector2D& InAttackPoint)
{
	if (!HasAuthority())
		return;

	bool bAccepted = false;

	UWorld* WorldPtr = GetWorld();
	if (IsValid(WorldPtr))
	{
		AJoustGameMode* GameModePtr = WorldPtr->GetAuthGameMode<AJoustGameMode>();
		if (IsValid(GameModePtr))
		{
			bAccepted =	GameModePtr->SubmitPlayerAttack(this, InAttackType, InAttackPoint);
		}
	}

	ClientAttackRequestCompleted(bAccepted);
}

void AJoustPlayerController::ServerRequestAttack_Implementation(EJoustAttackType InAttackType, FVector2D InAttackPoint)
{
	HandleAttackRequest(InAttackType, InAttackPoint);
}

void AJoustPlayerController::ClientAttackRequestCompleted_Implementation(bool bInAccepted)
{
	AttackRequestCompletedEvent.Broadcast(bInAccepted);
}

void AJoustPlayerController::HandleStartMatchRequest()
{
	UWorld* WorldPtr = GetWorld();
	if (!IsValid(WorldPtr))
		return;

	AJoustGameMode* GameModePtr = WorldPtr->GetAuthGameMode<AJoustGameMode>();
	if (!IsValid(GameModePtr))
		return;

	GameModePtr->StartJoustMatch(this);
}

void AJoustPlayerController::HandleDefenseRequest(const FVector2D& InShieldPoint, bool bInParryAttempted, float InParryInputTime)
{
	if (!HasAuthority() || !FMath::IsFinite(InShieldPoint.X) || !FMath::IsFinite(InShieldPoint.Y) || (bInParryAttempted && !FMath::IsFinite(InParryInputTime)))
		return;

	UWorld* WorldPtr = GetWorld();
	if (!IsValid(WorldPtr))
		return;

	AJoustGameMode* GameModePtr = WorldPtr->GetAuthGameMode<AJoustGameMode>();
	if (!IsValid(GameModePtr))
		return;

	GameModePtr->SubmitPlayerDefense(this, InShieldPoint, bInParryAttempted, InParryInputTime);
}

void AJoustPlayerController::ServerRequestParry_Implementation(FVector2D InShieldPoint, float InParryInputTime)
{
	HandleDefenseRequest(InShieldPoint, true, InParryInputTime);
}

void AJoustPlayerController::ServerRequestShieldPoint_Implementation(FVector2D InShieldPoint)
{
	HandleDefenseRequest(InShieldPoint, false, 0.0f);
}

void AJoustPlayerController::ServerRequestStartMatch_Implementation()
{
	HandleStartMatchRequest();
}
