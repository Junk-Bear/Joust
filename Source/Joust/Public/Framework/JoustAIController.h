// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Common/JoustCommonTypes.h"
#include "Interface/JoustAttackInput.h"
#include "Interface/JoustDefenseInput.h"
#include "Interface/JoustStrategyInput.h"
#include "JoustAIController.generated.h"

/**
 * AI 플레이어의 Strategy / Attack / Defense 입력 제공
 */
UCLASS()
class JOUST_API AJoustAIController : public AAIController, public IJoustStrategyInput, public IJoustAttackInput, public IJoustDefenseInput
{
	GENERATED_BODY()
	
public: // ########### 생성자용 블록 ##########

	AJoustAIController();

public: // ########### public 함수 블록 ##########

	// ====================
	// Strategy Input
	// ====================

	/** 선택한 전략 카드 ID를 저장 */
	void SetSelectedStrategyCardID(FName InCardID);

	/** 봉인할 전략 카드 ID를 저장 */
	void SetBannedStrategyCardID(FName InCardID);

	/** Strategy 입력을 초기화 */
	void ResetStrategyInput();

	/** IJoustStrategyInput을(를) 통해 상속됨 */
	bool TryGetSelectedStrategyCardID(FName& OutCardID) const override;
	bool TryGetBannedStrategyCardID(FName& OutCardID) const override;

	// ====================
	// Attack Input
	// ====================

	/** 현재 공격 지점을 저장 */
	void SetAttackPoint(const FVector2D& InAttackPoint);

	/** 현재 공격 타입을 저장 */
	void SetAttackType(EJoustAttackType InAttackType);

	/** 현재 공격 선택을 확정 */
	void ConfirmAttack();

	/** 현재 Attack 입력을 초기화 */
	void ResetAttackInput();

	/** IJoustAttackInput을(를) 통해 상속됨 */
	FVector2D GetAttackPoint() const override;
	EJoustAttackType GetAttackType() const override;
	bool IsAttackConfirmed() const override;

	// ====================
	// Defense Input
	// ====================

	/** 현재 방패 중심위치를 저장 */
	void SetShieldPoint(const FVector2D& InShieldPoint);

	/** 패링 입력과 입력 시간을 저장 */
	void SetParryAttempt(float InParryInputTime);

	/** 현재 Defense입력을 초기화 */
	void ResetDefenseInput();

	/** IJoustDefenseInput을(를) 통해 상속됨 */
	FVector2D GetShieldPoint() const override;
	bool IsParryAttempted() const override;
	float GetParryInputTime() const override;

private: // ########### private 변수 블록 ##########

	// ====================
	// Strategy Input
	// ====================

	FName SelectedStrategyCardID = NAME_None;

	FName BannedStrategyCardID = NAME_None;

	bool bHasSelectedStrategyCardID = false;

	bool bHasBannedStrategyCardID = false;

	// ====================
	// Attack Input
	// ====================

	FVector2D AttackPoint = FVector2D::ZeroVector;

	EJoustAttackType AttackType =
		EJoustAttackType::Normal;

	bool bAttackConfirmed = false;

	// ====================
	// Defense Input
	// ====================

	FVector2D ShieldPoint = FVector2D::ZeroVector;

	bool bParryAttempted = false;

	float ParryInputTime = 0.0f;
};
