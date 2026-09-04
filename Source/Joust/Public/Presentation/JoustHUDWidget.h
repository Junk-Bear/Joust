// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "JoustHUDWidget.generated.h"

class UTextBlock;
class UWidgetSwitcher;

/**
 * Joust Root HUD Widget의 C++ 기반 클래스
 */
UCLASS(Blueprintable)
class JOUST_API UJoustHUDWidget : public UUserWidget
{
	GENERATED_BODY()

private: // ########## Bind Widget 블록 ##########

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

public: // ########## GET SET 블록 ##########

	FORCEINLINE UWidgetSwitcher* GetPhaseSwitcher() const { return Switcher_Phase; }
};
