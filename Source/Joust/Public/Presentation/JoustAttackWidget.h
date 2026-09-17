// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/JoustPlayerTypes.h"
#include "JoustAttackWidget.generated.h"

class UButton;
class UTextBlock;
class UCanvasPanel;
class UImage;

/**
 * 공격 타입 및 AttackPoint 선택 화면
 */
UCLASS()
class JOUST_API UJoustAttackWidget : public UUserWidget
{
	GENERATED_BODY()

public: // ########## 델리게이트 블록 ##########

	/** 로컬 플레이어가 공격 종류와 지점을 확정 요청하는 이벤트 */
	DECLARE_EVENT_TwoParams(UJoustAttackWidget, FOnAttackConfirmed, EJoustAttackType, FVector2D)

public: // ########## public 함수 블록 ##########

	/** 복제된 공격 종류별 남은 사용 횟수를 버튼과 문구에 반영 */
	void SetAttackUsageStates(const TArray<FJoustAttackUsageState>& InUsageStates);

	/** RuleSet의 공격 가능 좌표 범위 설정 */
	void SetLanceBoxBounds(const FVector2D& InLanceBoxMin, const FVector2D& InLanceBoxMax);

	/** Attack 단계 진입 시 공격 종류와 지점 선택 초기화 */
	void ResetAttackSelection();

	/** 서버의 공격 요청 처리 결과를 화면에 반영 */
	void SetAttackRequestResult(bool bInAccepted);

protected: // ########## protected 함수 블록 ##########

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private: // ########## private 함수 블록 ##########

	/** 지정한 공격 종류의 복제된 남은 사용 횟수 반환 */
	int32 GetRemainingUses(EJoustAttackType InAttackType) const;

	/** 지정한 공격 종류를 선택할 수 있는지 확인 */
	bool CanSelectAttackType(EJoustAttackType InAttackType) const;

	/** 공격 종류 한 개의 사용 횟수와 버튼 상태 갱신 */
	void RefreshAttackOption(EJoustAttackType InAttackType, UButton* InButton, UTextBlock* InUsesText);

	/** 현재 선택한 공격 종류를 버튼 색상에 반영 */
	void UpdateAttackSelectionVisuals();

	/** 공격 종류와 지점 선택 상태에 따라 확정 버튼 갱신 */
	void UpdateConfirmButton();

	/** 지정한 공격 종류 선택 */
	void SelectAttackType(EJoustAttackType InAttackType);

	/** 일반 공격 버튼 클릭 처리 */
	UFUNCTION()
	void HandleNormalAttackClicked();

	/** 강공 버튼 클릭 처리 */
	UFUNCTION()
	void HandleStrongAttackClicked();

	/** 트릭 공격 버튼 클릭 처리 */
	UFUNCTION()
	void HandleTrickAttackClicked();

	/** 느린 공격 버튼 클릭 처리 */
	UFUNCTION()
	void HandleSlowAttackClicked();

	/** 공격 확정 버튼 클릭 처리 */
	UFUNCTION()
	void HandleConfirmClicked();

private: // ########## private 변수 블록 ##########

	/** 화면에 표시할 공격 종류별 남은 사용 횟수 */
	TArray<FJoustAttackUsageState> AttackUsageStates;

	/** 현재 선택한 공격 종류 */
	EJoustAttackType SelectedAttackType = EJoustAttackType::Normal;

	/** 현재 선택한 공격 지점 */
	FVector2D SelectedAttackPoint = FVector2D::ZeroVector;

	/** 공격 종류 선택 여부 */
	bool bHasSelectedAttackType = false;

	/** 공격 지점 선택 여부 */
	bool bHasSelectedAttackPoint = false;

	/** 유효한 LanceBox 범위가 설정됐는지 */
	bool bBoundsConfigured = false;

	/** 선택 가능한 최소 공격 좌표 */
	FVector2D LanceBoxMin = FVector2D::ZeroVector;

	/** 선택 가능한 최대 공격 좌표 */
	FVector2D LanceBoxMax = FVector2D::ZeroVector;

	/** 선택되지 않은 공격 버튼 색상 */
	UPROPERTY(EditDefaultsOnly, Category = "Joust|Attack|Appearance")
	FLinearColor NormalAttackColor = FLinearColor::White;

	/** 선택된 공격 버튼 색상 */
	UPROPERTY(EditDefaultsOnly, Category = "Joust|Attack|Appearance")
	FLinearColor SelectedAttackColor = FLinearColor(1.0f, 0.65f, 0.15f, 1.0f);

	/** FOnAttackConfirmed 이벤트용 */
	FOnAttackConfirmed AttackConfirmedEvent;

	/** 공격 요청을 서버에 전송하고 응답을 기다리는 중인지 */
	bool bAttackRequestPending = false;

	/** 서버가 현재 공격 입력을 승인했는지 */
	bool bAttackSubmitted = false;

public: // ########## GET SET 블록 ##########

	/** 공격 확정 이벤트 구독 */
	FORCEINLINE FOnAttackConfirmed& OnAttackConfirmed() { return AttackConfirmedEvent; }

private: // ########## Bind Widget 블록 ##########

	/** 일반 공격 선택 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_NormalAttack = nullptr;

	/** 강공 선택 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_StrongAttack = nullptr;

	/** 트릭 공격 선택 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_TrickAttack = nullptr;

	/** 느린 공격 선택 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_SlowAttack = nullptr;

	/** 일반 공격 남은 사용 횟수 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_NormalUses = nullptr;

	/** 강공 남은 사용 횟수 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_StrongUses = nullptr;

	/** 트릭 공격 남은 사용 횟수 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_TrickUses = nullptr;

	/** 느린 공격 남은 사용 횟수 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_SlowUses = nullptr;

	/** 클릭할 LanceBox 영역 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> Canvas_LanceBox = nullptr;

	/** 현재 선택한 공격 지점의 마커 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_AttackPointMarker = nullptr;

	/** 공격 확정 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Confirm = nullptr;

	/** 공격 확정 버튼 문구 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Confirm = nullptr;
};
