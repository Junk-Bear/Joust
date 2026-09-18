// Fill out your copyright notice in the Description page of Project Settings.

#include "Presentation/JoustDefenseWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Input/Events.h"
#include "Input/Reply.h"
#include "InputCoreTypes.h"
#include "Layout/Geometry.h"
#include "Rendering/DrawElementTypes.h"

void UJoustDefenseWidget::SetLanceBoxBounds(const FVector2D& InLanceBoxMin, const FVector2D& InLanceBoxMax)
{
	LanceBoxMin = InLanceBoxMin;
	LanceBoxMax = InLanceBoxMax;

	bBoundsConfigured =
		FMath::IsFinite(LanceBoxMin.X) &&
		FMath::IsFinite(LanceBoxMin.Y) &&
		FMath::IsFinite(LanceBoxMax.X) &&
		FMath::IsFinite(LanceBoxMax.Y) &&
		LanceBoxMin.X < LanceBoxMax.X &&
		LanceBoxMin.Y < LanceBoxMax.Y;

	ResetDefenseInput();
}

void UJoustDefenseWidget::ResetDefenseInput()
{
	CurrentShieldPoint = FVector2D::ZeroVector;
	LastSubmittedShieldPoint = FVector2D::ZeroVector;

	bHasShieldPoint = false;
	bHasSubmittedShieldPoint = false;
	bParryRequested = false;

	PredictionState = FJoustPredictionState{};

	InvalidateLayoutAndVolatility();

	if (IsValid(Image_Shield))
	{
		Image_Shield->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (IsValid(Text_DefenseHint))
	{
		Text_DefenseHint->SetText(NSLOCTEXT("JoustDefenseWidget", "DefenseHint", "방패 이동 : 마우스    패링 : 좌클릭"));
	}
}

void UJoustDefenseWidget::SetPredictionState(const FJoustPredictionState& InPredictionState)
{
	PredictionState = InPredictionState;

	InvalidateLayoutAndVolatility();
}

bool UJoustDefenseWidget::TryUpdateShieldPoint(const FVector2D& InScreenSpacePosition)
{
	if (!bBoundsConfigured || !IsValid(Canvas_LanceBox) || !IsValid(Image_Shield))
		return false;

	const FGeometry& LanceBoxGeometryRef = Canvas_LanceBox->GetCachedGeometry();

	const FVector2D CanvasSize = LanceBoxGeometryRef.GetLocalSize();

	if (CanvasSize.X <= 0.0f || CanvasSize.Y <= 0.0f)
		return false;

	const FVector2D LocalPosition =
		LanceBoxGeometryRef.AbsoluteToLocal(InScreenSpacePosition);

	if (LocalPosition.X < 0.0f || LocalPosition.Y < 0.0f || LocalPosition.X > CanvasSize.X || LocalPosition.Y > CanvasSize.Y)
		return false;

	UCanvasPanelSlot* ShieldSlotPtr = Cast<UCanvasPanelSlot>(Image_Shield->Slot);
	if (!IsValid(ShieldSlotPtr))
		return false;

	CurrentShieldPoint.X = FMath::Lerp(LanceBoxMin.X, LanceBoxMax.X, FMath::Clamp(LocalPosition.X / CanvasSize.X, 0.0f, 1.0f));

	CurrentShieldPoint.Y = FMath::Lerp(LanceBoxMin.Y, LanceBoxMax.Y, 1.0f - FMath::Clamp(LocalPosition.Y / CanvasSize.Y, 0.0f, 1.0f));

	bHasShieldPoint = true;

	ShieldSlotPtr->SetPosition(LocalPosition);

	Image_Shield->SetVisibility(ESlateVisibility::HitTestInvisible);

	return true;
}

FVector2D UJoustDefenseWidget::ConvertLancePointToPaintPosition(
	const FGeometry& InAllottedGeometry, 
	const FGeometry& InLanceBoxGeometry, 
	const FVector2D& InLancePoint) const
{
	const FVector2D CanvasOrigin = InAllottedGeometry.AbsoluteToLocal(InLanceBoxGeometry.LocalToAbsolute(FVector2D::ZeroVector));

	const FVector2D CanvasSize = InLanceBoxGeometry.GetLocalSize();

	const float NormalizedX = FMath::Clamp((InLancePoint.X - LanceBoxMin.X) / (LanceBoxMax.X - LanceBoxMin.X), 0.0f, 1.0f);

	const float NormalizedY = FMath::Clamp((InLancePoint.Y - LanceBoxMin.Y) / (LanceBoxMax.Y - LanceBoxMin.Y), 0.0f, 1.0f);

	return CanvasOrigin + FVector2D(NormalizedX * CanvasSize.X, (1.0f - NormalizedY) * CanvasSize.Y); 
}

FReply UJoustDefenseWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!TryUpdateShieldPoint(InMouseEvent.GetScreenSpacePosition()))
		return Super::NativeOnMouseMove(InGeometry, InMouseEvent);

	if (!bHasSubmittedShieldPoint || FVector2D::Distance(CurrentShieldPoint, LastSubmittedShieldPoint) >= FMath::Max(0.0f, ShieldPointSubmitDistance))
	{
		LastSubmittedShieldPoint = CurrentShieldPoint;
		bHasSubmittedShieldPoint = true;

		ShieldPointChangedEvent.Broadcast(CurrentShieldPoint);
	}

	return FReply::Handled();
}

FReply UJoustDefenseWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (
		InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton || bParryRequested)
	{
		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	}

	if (!TryUpdateShieldPoint(InMouseEvent.GetScreenSpacePosition()))
		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	bParryRequested = true;

	LastSubmittedShieldPoint = CurrentShieldPoint;
	bHasSubmittedShieldPoint = true;

	if (IsValid(Text_DefenseHint))
	{
		Text_DefenseHint->SetText(NSLOCTEXT("JoustDefenseWidget", "ParryRequested", "PARRY!"));
	}

	ParryRequestedEvent.Broadcast(CurrentShieldPoint);

	return FReply::Handled();
}

int32 UJoustDefenseWidget::NativePaint(
	const FPaintArgs& InArgs, 
	const FGeometry& InAllottedGeometry, 
	const FSlateRect& InCullingRect, 
	FSlateWindowElementList& OutDrawElements, 
	int32 InLayerId, 
	const FWidgetStyle& InWidgetStyle, 
	bool bInParentEnabled) const
{
	const int32 MaxLayerId = Super::NativePaint(InArgs, InAllottedGeometry, InCullingRect, OutDrawElements, InLayerId, InWidgetStyle, bInParentEnabled);

	if (!IsValid(Canvas_LanceBox) || !bBoundsConfigured)
		return MaxLayerId;

	if (!PredictionState.bIsPredictionVisible && !PredictionState.bIsAttackPointRevealed)
		return MaxLayerId;

	const FGeometry& LanceBoxGeometryRef = Canvas_LanceBox->GetCachedGeometry();

	const FVector2D CanvasSize = LanceBoxGeometryRef.GetLocalSize();

	if (CanvasSize.X <= 0.0f || CanvasSize.Y <= 0.0f)
		return MaxLayerId;

	const float LanceBoxWidth = LanceBoxMax.X - LanceBoxMin.X;
	const float LanceBoxHeight = LanceBoxMax.Y - LanceBoxMin.Y;

	if (LanceBoxWidth <= 0.0f || LanceBoxHeight <= 0.0f)
		return MaxLayerId;

	const float RadiusScaleX = CanvasSize.X / LanceBoxWidth;
	const float RadiusScaleY = CanvasSize.Y / LanceBoxHeight;

	const int32 CircleSegments = FMath::Max(8, PredictionCircleSegments);

	const int32 PredictionLayerId = MaxLayerId + 1;

	const FPaintGeometry PaintGeometry = InAllottedGeometry.ToPaintGeometry();

	if (PredictionState.bIsPredictionVisible)
	{
		for (const FJoustPredictionDisplayCircle& Item : PredictionState.DisplayCircles)
		{
			if (!FMath::IsFinite(Item.Center.X) || !FMath::IsFinite(Item.Center.Y) || !FMath::IsFinite(Item.Radius) || Item.Radius <= 0.0f)
				continue;
			
			const FVector2D CircleCenter = ConvertLancePointToPaintPosition(InAllottedGeometry, LanceBoxGeometryRef, Item.Center);

			const float CircleRadiusX = Item.Radius * RadiusScaleX;
			const float CircleRadiusY = Item.Radius * RadiusScaleY;

			TArray<FVector2f> CirclePoints;

			CirclePoints.Reserve(CircleSegments + 1);

			for (int32 i = 0; i <= CircleSegments; ++i)
			{
				const float Angle = 2.0f * PI * static_cast<float>(i) / static_cast<float>(CircleSegments);

				CirclePoints.Add(FVector2f(
					static_cast<float>(CircleCenter.X + FMath::Cos(Angle) * CircleRadiusX),
					static_cast<float>(CircleCenter.Y + FMath::Sin(Angle) * CircleRadiusY)));
			}

			FSlateDrawElement::MakeLines(
				OutDrawElements,
				PredictionLayerId,
				PaintGeometry,
				CirclePoints,
				ESlateDrawEffect::None,
				PredictionCircleColor,
				true,
				FMath::Max(1.0f, PredictionCircleThickness));
		}
	}

	if (PredictionState.bIsAttackPointRevealed)
	{
		const FVector2D AttackPointPosition = ConvertLancePointToPaintPosition(InAllottedGeometry, LanceBoxGeometryRef, PredictionState.RevealedAttackPoint);

		const float MarkerSize = FMath::Max(1.0f, RevealedAttackPointMarkerSize);

		TArray<FVector2f> FirstLinePoints;

		FirstLinePoints.Reserve(2);

		FirstLinePoints.Add(FVector2f(static_cast<float>(AttackPointPosition.X - MarkerSize), static_cast<float>(AttackPointPosition.Y - MarkerSize)));

		FirstLinePoints.Add(FVector2f(static_cast<float>(AttackPointPosition.X + MarkerSize), static_cast<float>(AttackPointPosition.Y + MarkerSize)));

		TArray<FVector2f> SecondLinePoints;

		SecondLinePoints.Reserve(2);

		SecondLinePoints.Add(FVector2f(static_cast<float>(AttackPointPosition.X - MarkerSize), static_cast<float>(AttackPointPosition.Y + MarkerSize)));

		SecondLinePoints.Add(FVector2f(static_cast<float>(AttackPointPosition.X + MarkerSize), static_cast<float>(AttackPointPosition.Y - MarkerSize)));

		const float MarkerThickness = FMath::Max(1.0f, RevealedAttackPointThickness);

		FSlateDrawElement::MakeLines(
			OutDrawElements,
			PredictionLayerId,
			PaintGeometry,
			FirstLinePoints,
			ESlateDrawEffect::None,
			RevealedAttackPointColor,
			true,
			MarkerThickness);

		FSlateDrawElement::MakeLines(
			OutDrawElements,
			PredictionLayerId,
			PaintGeometry,
			SecondLinePoints,
			ESlateDrawEffect::None,
			RevealedAttackPointColor,
			true,
			MarkerThickness);
	}

	return PredictionLayerId;
}
