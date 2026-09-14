// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Common/JoustCommonTypes.h"
#include "Interface/JoustAttackInput.h"
#include "Interface/JoustDefenseInput.h"
#include "Interface/JoustStrategyInput.h"
#include "JoustPlayerController.generated.h"

/**
 * 플레이어의 Strategy / Attack / Defense 입력 제공
 */
UCLASS()
class JOUST_API AJoustPlayerController : public APlayerController, public IJoustAttackInput, public IJoustDefenseInput, public IJoustStrategyInput
{
	GENERATED_BODY()

public: // ########### public 함수 블록 ##########

	/** Local UI 에서 경기 시작을 요청 */
	void RequestStartMatch();

	/** 선택한 전략 카드 ID를 저장 */
	void SetSelectedStrategyCardID(FName InCardID);

	/** 봉인할 전략 카드 ID를 저장 */
	void SetBannedStrategyCardID(FName InCardID);

	/** Strategy 입력을 초기화 */
	void ResetStrategyInput();

	/** IJoustStrategyInput을(를) 통해 상속됨 */
	bool TryGetSelectedStrategyCardID(FName& OutCardID) const override;
	bool TryGetBannedStrategyCardID(FName& OutCardID) const override;

	/** Local UI에서 선택한 전략 카드를 서버에 제출 요청 */
	void RequestStrategySelection(FName InCardID);

	/** 로컬 플레이어가 Strategy 카드 봉인을 요청 */
	void RequestStrategyBan(FName InCardID);

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

protected: // ########### protected 함수 블록

	/** Strategy 카드 봉인 요청을 서버로 전달 */
	UFUNCTION(Server, Reliable)
	void ServerRequestStrategyBan(FName InCardID);


private: // ########## private 함수 블록 ##########

	/** 서버에서 Strategy 선택 요청 처리 */
	void HandleStrategySelectionRequest(FName InCardID);

	/** 선택한 전략 카드 ID를 서버에 전달 */
	UFUNCTION(Server, Reliable)
	void ServerRequestStrategySelection(FName InCardID);

	/** 서버에서 Strategy 카드 봉인 요청 처리 */
	void HandleStrategyBanRequest(FName InCardID);

private: // ########### private 변수 블록 ##########

	/** 서버에서 실제 경기 시작 요청 처리 */
	void HandleStartMatchRequest();

	/** 클라이언트의 경기 시작 요청을 서버로 전달 */
	UFUNCTION(Server, Reliable)
	void ServerRequestStartMatch();

	FName SelectedStrategyCardID = NAME_None;

	FName BannedStrategyCardID = NAME_None;

	bool bHasSelectedStrategyCardID = false;

	bool bHasBannedStrategyCardID = false;

	FVector2D AttackPoint = FVector2D::ZeroVector;

	EJoustAttackType AttackType =
		EJoustAttackType::Normal;

	bool bAttackConfirmed = false;

	FVector2D ShieldPoint = FVector2D::ZeroVector;

	bool bParryAttempted = false;

	float ParryInputTime = 0.0f;
};
