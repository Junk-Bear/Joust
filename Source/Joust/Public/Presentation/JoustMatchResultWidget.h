// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Result/JoustResultTypes.h"
#include "JoustMatchResultWidget.generated.h"

class UBorder;
class UTextBlock;

/**
 * 최종 경기 결과 표시
 */
UCLASS()
class JOUST_API UJoustMatchResultWidget : public UUserWidget
{
	GENERATED_BODY()
	
public: // ########## public 함수 블록 ##########

	/** Local Player 기준 최종 경기 결과 표시 */
	void SetMatchResult(const FJoustMatchResult& InMatchResult, bool bInLocalPlayerA);

private: // ########## private 함수 블록 ##########

	/** 최종 MatchOutcome을 Local Player 기준 문구로 변환 */
	FText GetLocalOutcomeText(EJoustMatchOutcome InMatchOutcome, bool bInLocalPlayerA) const;

private: // ########## Bind Widget 블록 ##########

	/** 최종 경기 결과 패널 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> Border_MatchResultPanel = nullptr;

	/** Local Player 기준 승패 문구 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_MatchOutcome = nullptr;

	/** Player A 최종 점수 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_PlayerAScore = nullptr;

	/** Player B 최종 점수 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_PlayerBScore = nullptr;
};
