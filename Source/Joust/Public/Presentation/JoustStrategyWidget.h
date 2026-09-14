// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/JoustPlayerTypes.h"
#include "JoustStrategyWidget.generated.h"

class UButton;
class UTextBlock;
class UJoustRuleSetDataAsset;
class UOverlay;
class UJoustAttackHistoryWidget;
class UJoustDefenseHistoryWidget;

enum class EJoustStrategyInteractionMode : uint8
{
	Selection,
	Ban,
	Waiting
};

/**
 * 전략 카드 선택 및 카드 봉인 화면
 */
UCLASS()
class JOUST_API UJoustStrategyWidget : public UUserWidget
{
	GENERATED_BODY()

public: // ########## 델리게이트 블록 ##########
	/** Local Player가 선택한 전략 카드 확정 요청하는 이벤트 */
	DECLARE_EVENT_OneParam(UJoustStrategyWidget, FOnStrategyConfirmed, FName)
	DECLARE_EVENT_OneParam(UJoustStrategyWidget, FOnStrategyBanConfirmed, FName);

public: // ########## public 함수 블록 ##########

	/** 공개된 전략 카드 ID를 카드 이름과 효과 설명으로 표시 */
	void SetStrategyCards(const TArray<FName>& InCardIDs, const UJoustRuleSetDataAsset* InRuleSet);

	/** 현재 카드 선택 상태 초기화 */
	void ResetCardSelection();

	/** 현재 공개 봉인 상태에 맞춰 Strategy 상호작용 모드 설정 */
	void SetStrategyInteractionState(bool bInLocalBanPending, bool bInAnyBanPending, FName InBannedCardIDForPlayer);

	/** 로컬 플레이어의 공격·수비 기록 갱신 */
	void SetHistoryData(
		const TArray<FJoustAttackHistory>& InAttackHistory,
		const TArray<FJoustDefenseHistory>& InDefenseHistory,
		const FVector2D& InLanceBoxMin,
		const FVector2D& InLanceBoxMax);

protected: // ########## protected 함수 블록 ##########

	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

private : // ########## private 함수 블록 ###########

	/** 첫 번째 전략 카드 클릭 처리 */
	UFUNCTION()
	void HandleStrategyCard1Clicked();

	/** 두 번째 전략 카드 클릭 처리 */
	UFUNCTION()
	void HandleStrategyCard2Clicked();

	/** 세 번째 전략 카드 클릭 처리 */
	UFUNCTION()
	void HandleStrategyCard3Clicked();

	/** 네 번째 전략 카드 클릭 처리 */
	UFUNCTION()
	void HandleStrategyCard4Clicked();

	/** 다섯 번째 전략 카드 클릭 처리 */
	UFUNCTION()
	void HandleStrategyCard5Clicked();

	/** 지정한 슬롯을 현재 선택 카드로 설정 */
	void HandleStrategyCardClicked(int32 InSlotIdx);

	/** 현재 선택 인덱스를 카드 버튼 색상에 반영 */
	void UpdateCardSelectionVisuals();

	/** 전략 확정 버튼 클릭 처리 */
	UFUNCTION()
	void HandleStrategyConfirmClicked();

	/** 현재 모드에 맞춰 안내 문구와 버튼 상태 갱신 */
	void RefreshInteractionState();

	/** 해당 카드가 현재 모드에서 선택 가능한지 확인 */
	bool IsCardSelectable(int32 InCardIdx) const;

	/** 공격 히스토리 팝업 열기 */
	UFUNCTION()
	void HandleAttackHistoryClicked();

	/** 수비 히스토리 팝업 열기 */
	UFUNCTION()
	void HandleDefenseHistoryClicked();

	/** 두 히스토리 팝업 닫기 */
	void CloseHistoryPopups();


private: // ########## private 변수 블록 ##########

	/** 인덱스로 카드 버튼을 처리하기 위한 슬롯 배열 */
	UPROPERTY(Transient)
	TArray<TObjectPtr<UButton>> CardButtons;

	/** 인덱스로 카드 이름 Text를 처리하기 위한 슬롯 배열 */
	UPROPERTY(Transient)
	TArray<TObjectPtr<UTextBlock>> CardNameTexts;

	/** 인덱스로 카드 효과 Text를 처리하기 위한 슬롯 배열 */
	UPROPERTY(Transient)
	TArray<TObjectPtr<UTextBlock>> CardEffectTexts;

	/** 현재 각 화면 슬롯에 표시된 실제 카드 ID */
	TArray<FName> DisplayedCardIDs;

	/** 현재 선택된 카드의 화면 슬롯 인덱스 */
	int32 SelectedCardIdx = INDEX_NONE;

	/** 선택되지 않은 카드 버튼의 기본 색상 */
	UPROPERTY(EditDefaultsOnly, Category = "Joust|Strategy|Appearance")
	FLinearColor NormalCardColor = FLinearColor::White;

	/** 선택된 카드 버튼의 강조 색상 */
	UPROPERTY(EditDefaultsOnly, Category = "Joust|Strategy|Appearance")
	FLinearColor SelectedCardColor = FLinearColor(1.0f, 0.65f, 0.15f, 1.0f);

	/** FOnStrategyConfirmed 이벤트용 */
	FOnStrategyConfirmed StrategyConfirmedEvent;

	/** 현재 Strategy 입력 모드 */
	EJoustStrategyInteractionMode InteractionMode = EJoustStrategyInteractionMode::Selection;

	/** 플레이어에게 봉인된 카드 */
	FName BannedCardIDForPlayer = NAME_None;

	/** FOnStrategyBanConfirmed 이벤트용 */
	FOnStrategyBanConfirmed StrategyBanConfirmedEvent;

	public: // ########## GET SET 블록 ##########

		FORCEINLINE FOnStrategyConfirmed& OnStrategyConfirmed() { return StrategyConfirmedEvent; }

		FORCEINLINE FOnStrategyBanConfirmed& OnStrategyBanConfirmed() { return StrategyBanConfirmedEvent; }


private: // ########## Bind Widget 블록 ##########

	/** 첫 번째 전략 카드 선택 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_StrategyCard1 = nullptr;

	/** 두 번째 전략 카드 선택 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_StrategyCard2 = nullptr;

	/** 세 번째 전략 카드 선택 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_StrategyCard3 = nullptr;

	/** 네 번째 전략 카드 선택 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_StrategyCard4 = nullptr;

	/** 다섯 번째 전략 카드 선택 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_StrategyCard5 = nullptr;

	/** 첫 번째 전략 카드의 표시 이름 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Card1Name = nullptr;

	/** 두 번째 전략 카드의 표시 이름 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Card2Name = nullptr;

	/** 세 번째 전략 카드의 표시 이름 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Card3Name = nullptr;

	/** 네 번째 전략 카드의 표시 이름 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Card4Name = nullptr;

	/** 다섯 번째 전략 카드의 표시 이름 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Card5Name = nullptr;

	/** 첫 번째 전략 카드의 효과 설명 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Card1Effect = nullptr;

	/** 두 번째 전략 카드의 효과 설명 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Card2Effect = nullptr;

	/** 세 번째 전략 카드의 효과 설명 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Card3Effect = nullptr;

	/** 네 번째 전략 카드의 효과 설명 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Card4Effect = nullptr;

	/** 다섯 번째 전략 카드의 효과 설명 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Card5Effect = nullptr;

	/** 현재 선택한 전략 카드를 확정하는 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_StrategyConfirm = nullptr;

	/** Strategy 단계 안내 문구 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_StrategyInstruction = nullptr;

	/** 공격 히스토리 열기 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_AttackHistory = nullptr;

	/** 수비 히스토리 열기 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_DefenseHistory = nullptr;

	/** 봉인 입력 대기 화면 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> Overlay_BanWaiting = nullptr;

	/** 봉인 입력 대기 문구 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_BanWaitingMessage = nullptr;

	/** 공격 히스토리 팝업 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UJoustAttackHistoryWidget>
		WBP_JoustAttackHistory = nullptr;

	/** 수비 히스토리 팝업 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UJoustDefenseHistoryWidget>
		WBP_JoustDefenseHistory = nullptr;
};
