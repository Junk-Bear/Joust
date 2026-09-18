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
class JOUST_API AJoustPlayerController : public APlayerController, public IJoustStrategyInput, public IJoustAttackInput, public IJoustDefenseInput
{
	GENERATED_BODY()

public: // ########## 델리게이트 블록 ##########

	/** 서버의 공격 요청 처리 결과 이벤트 */
	DECLARE_EVENT_OneParam(AJoustPlayerController, FOnAttackRequestCompleted, bool);

public: // ########### public 함수 블록 ##########

	/** Local UI 에서 경기 시작을 요청 */
	void RequestStartMatch();

	/** 선택한 전략 카드 ID를 저장 */
	void SetSelectedStrategyCardID(FName InCardID);

	/** 봉인할 전략 카드 ID를 저장 */
	void SetBannedStrategyCardID(FName InCardID);

	/** Strategy 입력을 초기화 */
	void ResetStrategyInput();

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

	/** 현재 방패 중심위치를 저장 */
	void SetShieldPoint(const FVector2D& InShieldPoint);

	/** 패링 입력과 입력 시간을 저장 */
	void SetParryAttempt(float InParryInputTime);

	/** 현재 Defense입력을 초기화 */
	void ResetDefenseInput();

	/** Local UI에서 확정한 플레이어 공격 타입과 지점을 서버에 제출 요청 */
	void RequestAttack(EJoustAttackType InAttackType, const FVector2D& InAttackPoint);

	/** Local UI의 방패 위치를 서버에 제출 요청 */
	void RequestShieldPoint(const FVector2D& InShieldPoint);

	/** Local UI의 패링 입력을 서버에 제출 요청 */
	void RequestParry(const FVector2D& InShieldPoint, float InParryInputTime);

	// ====================
	// IJoustStrategyInput 상속됨
	// ====================
	bool TryGetSelectedStrategyCardID(FName& OutCardID) const override;
	bool TryGetBannedStrategyCardID(FName& OutCardID) const override;

	// ====================
	// IJoustAttackInput 상속됨
	// ====================
	FVector2D GetAttackPoint() const override;
	EJoustAttackType GetAttackType() const override;
	bool IsAttackConfirmed() const override;

	// ====================
	// IJoustDefenseInput 상속됨
	// ====================
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

	/** 서버에서 플레이어 Attack 요청을 GameMode에 전달 */
	void HandleAttackRequest(EJoustAttackType InAttackType, const FVector2D& InAttackPoint);

	/** 확정한 플레이어 Attack 입력을 서버로 전달 */
	UFUNCTION(Server, Reliable)
	void ServerRequestAttack(EJoustAttackType InAttackType, FVector2D InAttackPoint);

	/** 소유 클라이언트에 공격 요청 처리 결과 전달 */
	UFUNCTION(Client, Reliable)
	void ClientAttackRequestCompleted(bool bInAccepted);

	/** 서버에서 실제 경기 시작 요청 처리 */
	void HandleStartMatchRequest();

	/** 클라이언트의 경기 시작 요청을 서버로 전달 */
	UFUNCTION(Server, Reliable)
	void ServerRequestStartMatch();

	/** 서버에서 Player의 Defense 입력 처리 */
	void HandleDefenseRequest(const FVector2D& InShieldPoint, bool bInParryAttempted, float InParryInputTime);

	/** 변경된 방패 위치를 서버에 전달 */
	UFUNCTION(Server, Unreliable)
	void ServerRequestShieldPoint(FVector2D InShieldPoint);

	/** 패링 위치와 입력 시간을 서버에 전달 */
	UFUNCTION(Server, Reliable)
	void ServerRequestParry(FVector2D InShieldPoint, float InParryInputTime);

private: // ########### private 변수 블록 ##########

	/** 현재 선택한 전략 카드 ID */
	FName SelectedStrategyCardID = NAME_None;

	/** 현재 선택한 봉인 대상 전략 카드 ID */
	FName BannedStrategyCardID = NAME_None;

	/** 전략 카드 선택 완료 여부 */
	bool bHasSelectedStrategyCardID = false;

	/** 봉인 대상 전략 카드 선택 완료 여부 */
	bool bHasBannedStrategyCardID = false;

	/** 현재 선택한 공격 지점 */
	FVector2D AttackPoint = FVector2D::ZeroVector;

	/** 현재 선택한 공격 종류 */
	EJoustAttackType AttackType = EJoustAttackType::Normal;

	/** 현재 공격 입력 확정 여부 */
	bool bAttackConfirmed = false;

	/** 현재 선택한 방패 중심 위치 */
	FVector2D ShieldPoint = FVector2D::ZeroVector;

	/** 현재 라운드에서 패링을 시도했는지 */
	bool bParryAttempted = false;

	/** 현재 라운드의 패링 입력 시간 */
	float ParryInputTime = 0.0f;

	/** FOnAttackRequestCompleted 이벤트용 */
	FOnAttackRequestCompleted AttackRequestCompletedEvent;

public: // ########## GET SET 블록 ##########

		FORCEINLINE FOnAttackRequestCompleted& OnAttackRequestCompleted() { return AttackRequestCompletedEvent; }
};
