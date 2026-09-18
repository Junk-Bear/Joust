// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Prediction/JoustPredictionTypes.h"
#include "JoustDefenseWidget.generated.h"

class UCanvasPanel;
class UImage;
class UTextBlock;

/**
 * Prediction, Shield, Parry 입력 화면
 */
UCLASS()
class JOUST_API UJoustDefenseWidget : public UUserWidget
{
	GENERATED_BODY()

public: // ########## 델리게이트 블록 ##########

	/** 방패 중심 위치가 변경된 이벤트 */
	DECLARE_EVENT_OneParam(UJoustDefenseWidget, FOnShieldPointChanged, FVector2D)

	/** Local Player가 패링을 요청한 이벤트 */
	DECLARE_EVENT_OneParam(UJoustDefenseWidget, FOnParryRequested, FVector2D)

public: // ########## public 함수 블록 ##########

	/** RuleSet의 방어 가능 좌표 범위 설정 */
	void SetLanceBoxBounds(
		const FVector2D& InLanceBoxMin,
		const FVector2D& InLanceBoxMax);

	/** Defense 단계 진입 시 방패와 패링 상태 초기화 */
	void ResetDefenseInput();

	/** 공개 Prediction 표시 상태 갱신 */
	void SetPredictionState(const FJoustPredictionState& InPredictionState);

protected: // ########## protected 함수 블록 ##########

	virtual FReply NativeOnMouseMove(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonDown(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent) override;

	virtual int32 NativePaint(
		const FPaintArgs& InArgs,
		const FGeometry& InAllottedGeometry,
		const FSlateRect& InCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 InLayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bInParentEnabled) const override;

private: // ########## private 함수 블록 ##########

	/** 화면 좌표를 방어 좌표로 변환하고 방패 위치 갱신 */
	bool TryUpdateShieldPoint(const FVector2D& InScreenSpacePosition);

	/** LanceBox 좌표를 Widget Paint 좌표로 변환 */
	FVector2D ConvertLancePointToPaintPosition(const FGeometry& InAllottedGeometry, const FGeometry& InLanceBoxGeometry, const FVector2D& InLancePoint) const;

private: // ########## private 변수 블록 ##########

	/** 현재 방패 중심 위치 */
	FVector2D CurrentShieldPoint = FVector2D::ZeroVector;

	/** 마지막으로 서버 전달 이벤트를 발생시킨 방패 위치 */
	FVector2D LastSubmittedShieldPoint = FVector2D::ZeroVector;

	/** 유효한 방패 위치가 선택됐는지 */
	bool bHasShieldPoint = false;

	/** 서버 전달 기준 위치가 존재하는지 */
	bool bHasSubmittedShieldPoint = false;

	/** 현재 Defense 단계에서 패링을 시도했는지 */
	bool bParryRequested = false;

	/** 유효한 LanceBox 범위가 설정됐는지 */
	bool bBoundsConfigured = false;

	/** 선택 가능한 최소 방어 좌표 */
	FVector2D LanceBoxMin = FVector2D::ZeroVector;

	/** 선택 가능한 최대 방어 좌표 */
	FVector2D LanceBoxMax = FVector2D::ZeroVector;

	/** 방패 위치 이벤트를 다시 발생시킬 최소 이동 거리 */
	UPROPERTY(EditDefaultsOnly, Category = "Joust|Defense|Input")
	float ShieldPointSubmitDistance = 0.02f;

	/** FOnShieldPointChanged 이벤트용 */
	FOnShieldPointChanged ShieldPointChangedEvent;

	/** FOnParryRequested 이벤트용 */
	FOnParryRequested ParryRequestedEvent;

	/** 현재 화면에 표시할 공개 Prediction 상태 */
	FJoustPredictionState PredictionState;

	/** Prediction 원 색상 */
	UPROPERTY(EditDefaultsOnly, Category = "Joust|Defense|Appearance")
	FLinearColor PredictionCircleColor = FLinearColor(0.15f, 0.65f, 1.0f, 0.9f);

	/** Prediction 원 선 두께 */
	UPROPERTY(EditDefaultsOnly, Category = "Joust|Defense|Appearance", meta = (ClampMin = "1.0"))
	float PredictionCircleThickness = 3.0f;

	/** Prediction 원을 구성할 선분 수 */
	UPROPERTY(EditDefaultsOnly, Category = "Joust|Defense|Appearance", meta = (ClampMin = "8"))
	int32 PredictionCircleSegments = 64;

	/** 공개된 실제 공격 지점 색상 */
	UPROPERTY(EditDefaultsOnly, Category = "Joust|Defense|Appearance")
	FLinearColor RevealedAttackPointColor = FLinearColor::Red;

	/** 실제 공격 지점 X 마커 크기 */
	UPROPERTY(EditDefaultsOnly, Category = "Joust|Defense|Appearance", meta = (ClampMin = "1.0"))
	float RevealedAttackPointMarkerSize = 12.0f;

	/** 실제 공격 지점 X 마커 선 두께 */
	UPROPERTY(EditDefaultsOnly, Category = "Joust|Defense|Appearance", meta = (ClampMin = "1.0"))
	float RevealedAttackPointThickness = 3.0f;

public: // ########## GET SET 블록 ##########

	FORCEINLINE FOnShieldPointChanged& OnShieldPointChanged() { return ShieldPointChangedEvent; }

	FORCEINLINE FOnParryRequested& OnParryRequested() { return ParryRequestedEvent; }

private: // ########## Bind Widget 블록 ##########

	/** 방패를 이동할 LanceBox 영역 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> Canvas_LanceBox = nullptr;

	/** 현재 방패 중심 위치 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Shield = nullptr;

	/** Defense 조작 안내 문구 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_DefenseHint = nullptr;
};
