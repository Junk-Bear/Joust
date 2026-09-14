// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/JoustPlayerTypes.h"
#include "JoustAttackHistoryWidget.generated.h"

class UButton;
class UCanvasPanel;
class UVerticalBox;

/**
 * 공격 히스토리 팝업
 */
UCLASS()
class JOUST_API UJoustAttackHistoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public: // ########## public 함수 블록 ##########

	/** 공격 기록을 LanceBox 위의 마커로 갱신 */
	void SetHistory(const TArray<FJoustAttackHistory>& InHistory, const FVector2D& InLanceBoxMin, const FVector2D& InLanceBoxMax);

protected: // ########## protected 함수 블록 ##########

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

private: // ########## private 함수 블록 ##########

	/** CLOSE 버튼 클릭 처리 */
	UFUNCTION()
	void HandleCloseClicked();

	/** 기존 마커와 겹침 설명 제거 */
	void ClearHistory();

	/** 논리 LanceBox 좌표를 600×600 Canvas 좌표로 변환 */
	FVector2D ConvertPointToCanvasPosition(const FVector2D& InPoint, const FVector2D& InLanceBoxMin, const FVector2D& InLanceBoxMax) const;

	/** 공격 타입에 대응하는 마커 색상 반환 */
	FLinearColor ResolveAttackTypeColor(EJoustAttackType InAttackType) const;

	/** 공격 타입의 표시 이름 반환 */
	FString ResolveAttackTypeText(EJoustAttackType InAttackType) const;

	/** Canvas에 공격 마커 추가 */
	void AddMarker(const FVector2D& InPosition, const FString& InLabel, const FLinearColor& InColor, bool bInOverlapMarker);

	/** LanceBox 아래에 겹침 설명 추가 */
	void AddOverlapDescription(const FString& InDescription);

private: // ########## private 변수 블록 ##########

	/** LanceBox의 UI 크기 */
	float MarkerCanvasSize = 600.0f;

	/** 마커가 차지하는 영역 */
	float MarkerSize = 40.0f;

	/** 두 마커를 겹침으로 처리할 중심 거리 */
	float OverlapDistance = 40.0f;

private: // ########## Bind Widget 블록 ##########

	/** 공격 기록 마커가 추가될 Canvas */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> Canvas_Markers = nullptr;

	/** 겹친 공격 기록의 설명이 추가될 영역 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> VerticalBox_OverlapDescriptions = nullptr;

	/** 공격 히스토리 닫기 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Close = nullptr;
};
