// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Result/JoustResultTypes.h"
#include "JoustRoundResultWidget.generated.h"

class UBorder;
class UTextBlock;

/**
 * 라운드 결과 표시 및 결과 HUD 애니메이션
 */
UCLASS()
class JOUST_API UJoustRoundResultWidget : public UUserWidget
{
	GENERATED_BODY()
public: // ########## public 함수 블록 ##########

	/** Local Player 기준 라운드 결과와 누적 점수 표시 */
	void SetRoundResult(const FJoustRoundResult& InRoundResult, bool bInLocalPlayerA, int32 InPlayerAScore, int32 InPlayerBScore);

private: // ########## private 함수 블록 ##########
	/** Local Player 공격 결과 문구 구성 */
	FText GetAttackResultText(const FJoustExchangeResult& InExchangeResult) const;

	/** Local Player 방어 결과 문구 구성 */
	FText GetDefenseResultText(const FJoustExchangeResult& InExchangeResult) const;

private: // ########## Bind Widget 블록 ##########

	/** 일반 라운드 결과 패널 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> Border_ResultPanel = nullptr;

	/** 현재 라운드 제목 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_RoundTitle = nullptr;

	/** Local Player 공격 결과 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_AttackValue = nullptr;

	/** Local Player 방어 결과 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_DefenseValue = nullptr;

	/** 이번 라운드 Local Player 득점 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_ScoreValue = nullptr;

	/** Player A/B 누적 점수 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_TotalValue = nullptr;

	/** 낙마 전용 결과 문구 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Unhorse = nullptr;
};
