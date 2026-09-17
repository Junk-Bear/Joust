// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Common/JoustCommonTypes.h"
#include "JoustHUDWidget.generated.h"

class UBorder;
class UTextBlock;
class UWidgetSwitcher;
class UJoustMatchStartWidget;
class UJoustStrategyWidget;
class UJoustAttackWidget;
class UJoustDefenseWidget;
class UJoustRoundResultWidget;
class UJoustMatchResultWidget;
class UJoustStrategyWidget;

/**
 * Joust Root HUD Widget의 C++ 기반 클래스
 */
UCLASS(Blueprintable)
class JOUST_API UJoustHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public: // ########## public 함수 블록 ##########

	/** 경기 시작 전 화면 표시*/
	void ShowMatchStart();

	/** 현재 Phase 화면 표시 */
	void ShowPhase(EJoustPhase InPhase, bool bInUnhorseResult);

	/** 라운드, Phase, 점수 등 갱신*/
	void UpdateCommonHUD(int32 InRoundNumber, int32 InPlayerAScore, int32 InPlayerBScore, EJoustPhase InPhase);

	/** 남은 시간 갱신(초 단위)*/
	void UpdateRemainingTime(int32 InRemainingSeconds);

public: // ########### GET SET 블록 ##########
	
	FORCEINLINE UJoustMatchStartWidget* GetMatchStartWidget() const { return WBP_JoustMatchStart; }

	FORCEINLINE UJoustStrategyWidget* GetStrategyWidget() const { return WBP_JoustStrategy; }

	FORCEINLINE UJoustAttackWidget* GetAttackWidget() const { return WBP_JoustAttack; }

private: // ########## Bind Widget 블록 ##########

	/** 상단 공통 HUD */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> Border_CommonHUD = nullptr;

	/** Player A 점수 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_PlayerAScore = nullptr;

	/** Player B 점수 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_PlayerBScore = nullptr;

	/** 현재 라운드 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Round = nullptr;

	/** 현재 Phase */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Phase = nullptr;

	/** 현재 Phase 남은 시간 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Timer = nullptr;

	/** Phase별 UI 화면 전환 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> Switcher_Phase = nullptr;

	/** 경기 시작 화면 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UJoustMatchStartWidget> WBP_JoustMatchStart = nullptr;

	/** Strategy 화면 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UJoustStrategyWidget> WBP_JoustStrategy = nullptr;

	/** Attack 화면 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UJoustAttackWidget> WBP_JoustAttack = nullptr;

	/** Defense 화면 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UJoustDefenseWidget> WBP_JoustDefense = nullptr;

	/** RoundResult 화면 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UJoustRoundResultWidget> WBP_JoustRoundResult = nullptr;

	/** MatchResult 화면 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UJoustMatchResultWidget> WBP_JoustMatchResult = nullptr;

};
