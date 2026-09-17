// Fill out your copyright notice in the Description page of Project Settings.

#include "Presentation/JoustAttackWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Input/Events.h"
#include "Input/Reply.h"
#include "InputCoreTypes.h"
#include "Layout/Geometry.h"

int32 UJoustAttackWidget::GetRemainingUses(EJoustAttackType InAttackType) const
{
	for (const FJoustAttackUsageState& Item : AttackUsageStates)
	{
		if (Item.AttackType == InAttackType)
		{
			return Item.RemainingUses;
		}
	}

	return 0;
}

bool UJoustAttackWidget::CanSelectAttackType(EJoustAttackType InAttackType) const
{
	const int32 RemainingUses = GetRemainingUses(InAttackType);

	return RemainingUses == INDEX_NONE || RemainingUses > 0;
}

void UJoustAttackWidget::RefreshAttackOption(EJoustAttackType InAttackType, UButton* InButton, UTextBlock* InUsesText)
{
	if (!IsValid(InButton) || !IsValid(InUsesText))
		return;

	const int32 RemainingUses = GetRemainingUses(InAttackType);

	InUsesText->SetText(RemainingUses == INDEX_NONE ? FText::FromString(TEXT("∞")) : FText::AsNumber(RemainingUses));

	const bool bHasRemainingUses = RemainingUses == INDEX_NONE || RemainingUses > 0;

	InButton->SetIsEnabled(bHasRemainingUses && !bAttackRequestPending && !bAttackSubmitted);
}

void UJoustAttackWidget::UpdateAttackSelectionVisuals()
{
	if (IsValid(Button_NormalAttack))
	{
		Button_NormalAttack->SetBackgroundColor(bHasSelectedAttackType && SelectedAttackType == EJoustAttackType::Normal ? SelectedAttackColor : NormalAttackColor);
	}

	if (IsValid(Button_StrongAttack))
	{
		Button_StrongAttack->SetBackgroundColor(bHasSelectedAttackType && SelectedAttackType == EJoustAttackType::Strong ? SelectedAttackColor : NormalAttackColor);
	}

	if (IsValid(Button_TrickAttack))
	{
		Button_TrickAttack->SetBackgroundColor(bHasSelectedAttackType && SelectedAttackType == EJoustAttackType::Trick ? SelectedAttackColor : NormalAttackColor);
	}

	if (IsValid(Button_SlowAttack))
	{
		Button_SlowAttack->SetBackgroundColor(bHasSelectedAttackType && SelectedAttackType == EJoustAttackType::Slow ? SelectedAttackColor : NormalAttackColor);
	}
}

void UJoustAttackWidget::UpdateConfirmButton()
{
	if (IsValid(Text_Confirm))
	{
		if (bAttackSubmitted)
		{
			Text_Confirm->SetText(FText::FromString(TEXT("WAITING FOR OPPONENT")));
		}
		else if (bAttackRequestPending)
		{
			Text_Confirm->SetText(FText::FromString(TEXT("SUBMITTING...")));
		}
		else
		{
			Text_Confirm->SetText(FText::FromString(TEXT("CONFIRM")));
		}
	}

	if (!IsValid(Button_Confirm))
		return;

	const bool bCanConfirm =
		!bAttackRequestPending &&
		!bAttackSubmitted &&
		bHasSelectedAttackType &&
		bHasSelectedAttackPoint &&
		bBoundsConfigured &&
		CanSelectAttackType(SelectedAttackType);

	Button_Confirm->SetIsEnabled(bCanConfirm);
}

void UJoustAttackWidget::ResetAttackSelection()
{
	bHasSelectedAttackType = false;
	bHasSelectedAttackPoint = false;
	bAttackRequestPending = false;
	bAttackSubmitted = false;

	SelectedAttackType = EJoustAttackType::Normal;
	SelectedAttackPoint = FVector2D::ZeroVector;

	if (IsValid(Image_AttackPointMarker))
	{
		Image_AttackPointMarker->SetVisibility(ESlateVisibility::Collapsed);
	}

	RefreshAttackOption(EJoustAttackType::Normal, Button_NormalAttack, Text_NormalUses);
	RefreshAttackOption(EJoustAttackType::Strong, Button_StrongAttack, Text_StrongUses);
	RefreshAttackOption(EJoustAttackType::Trick, Button_TrickAttack, Text_TrickUses);
	RefreshAttackOption(EJoustAttackType::Slow, Button_SlowAttack, Text_SlowUses);

	UpdateAttackSelectionVisuals();

	UpdateConfirmButton();
}

void UJoustAttackWidget::SetAttackRequestResult(bool bInAccepted)
{
	bAttackRequestPending = false;
	bAttackSubmitted = bInAccepted;

	RefreshAttackOption(EJoustAttackType::Normal, Button_NormalAttack, Text_NormalUses);
	RefreshAttackOption(EJoustAttackType::Strong, Button_StrongAttack, Text_StrongUses);
	RefreshAttackOption(EJoustAttackType::Trick, Button_TrickAttack, Text_TrickUses);
	RefreshAttackOption(EJoustAttackType::Slow, Button_SlowAttack, Text_SlowUses);

	UpdateConfirmButton();
}

void UJoustAttackWidget::SetAttackUsageStates(const TArray<FJoustAttackUsageState>& InUsageStates)
{
	AttackUsageStates = InUsageStates;

	RefreshAttackOption(EJoustAttackType::Normal, Button_NormalAttack, Text_NormalUses);
	RefreshAttackOption(EJoustAttackType::Strong, Button_StrongAttack, Text_StrongUses);
	RefreshAttackOption(EJoustAttackType::Trick, Button_TrickAttack, Text_TrickUses);
	RefreshAttackOption(EJoustAttackType::Slow, Button_SlowAttack, Text_SlowUses);

	if (bHasSelectedAttackType && !CanSelectAttackType(SelectedAttackType))
	{
		bHasSelectedAttackType = false;
	}

	UpdateAttackSelectionVisuals();
	UpdateConfirmButton();
}

void UJoustAttackWidget::SetLanceBoxBounds(const FVector2D& InLanceBoxMin, const FVector2D& InLanceBoxMax)
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

	bHasSelectedAttackPoint = false;

	if (IsValid(Image_AttackPointMarker))
	{
		Image_AttackPointMarker->SetVisibility(ESlateVisibility::Collapsed);
	}

	UpdateConfirmButton();
}

void UJoustAttackWidget::SelectAttackType(EJoustAttackType InAttackType)
{
	if (!CanSelectAttackType(InAttackType))
		return;

	SelectedAttackType = InAttackType;
	bHasSelectedAttackType = true;

	UpdateAttackSelectionVisuals();
	UpdateConfirmButton();
}

void UJoustAttackWidget::HandleNormalAttackClicked()
{
	SelectAttackType(EJoustAttackType::Normal);
}

void UJoustAttackWidget::HandleStrongAttackClicked()
{
	SelectAttackType(EJoustAttackType::Strong);
}

void UJoustAttackWidget::HandleTrickAttackClicked()
{
	SelectAttackType(EJoustAttackType::Trick);
}

void UJoustAttackWidget::HandleSlowAttackClicked()
{
	SelectAttackType(EJoustAttackType::Slow);
}

void UJoustAttackWidget::HandleConfirmClicked()
{
	if (
		bAttackRequestPending ||
		bAttackSubmitted ||
		!bHasSelectedAttackType ||
		!bHasSelectedAttackPoint ||
		!bBoundsConfigured ||
		!CanSelectAttackType(SelectedAttackType)
		)
		return;

	bAttackRequestPending = true;

	RefreshAttackOption(EJoustAttackType::Normal, Button_NormalAttack, Text_NormalUses);
	RefreshAttackOption(EJoustAttackType::Strong, Button_StrongAttack, Text_StrongUses);
	RefreshAttackOption(EJoustAttackType::Trick, Button_TrickAttack, Text_TrickUses);
	RefreshAttackOption(EJoustAttackType::Slow, Button_SlowAttack, Text_SlowUses);

	UpdateConfirmButton();

	AttackConfirmedEvent.Broadcast(SelectedAttackType, SelectedAttackPoint);
}

FReply UJoustAttackWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (
		bAttackRequestPending ||
		bAttackSubmitted ||
		InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton ||
		!IsValid(Canvas_LanceBox) ||
		!IsValid(Image_AttackPointMarker) ||
		!bBoundsConfigured
		)
	{
		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	}

	const FGeometry& LanceBoxGeometryRef = Canvas_LanceBox->GetCachedGeometry();

	const FVector2D CanvasSize = LanceBoxGeometryRef.GetLocalSize();

	if (CanvasSize.X <= 0.0f || CanvasSize.Y <= 0.0f)
		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	const FVector2D LocalPosition = LanceBoxGeometryRef.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

	if (LocalPosition.X < 0.0f ||
		LocalPosition.Y < 0.0f ||
		LocalPosition.X > CanvasSize.X ||
		LocalPosition.Y > CanvasSize.Y)
	{
		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	}

	UCanvasPanelSlot* MarkerSlotPtr = Cast<UCanvasPanelSlot>(Image_AttackPointMarker->Slot);

	if (!IsValid(MarkerSlotPtr))
		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	const float NormalizedX = FMath::Clamp(LocalPosition.X / CanvasSize.X, 0.0f, 1.0f);

	const float NormalizedY = FMath::Clamp(LocalPosition.Y / CanvasSize.Y, 0.0f, 1.0f);

	SelectedAttackPoint.X = FMath::Lerp(LanceBoxMin.X, LanceBoxMax.X, NormalizedX);
	SelectedAttackPoint.Y = FMath::Lerp(LanceBoxMin.Y, LanceBoxMax.Y, 1.0f - NormalizedY);

	bHasSelectedAttackPoint = true;

	MarkerSlotPtr->SetPosition(LocalPosition);

	Image_AttackPointMarker->SetVisibility(ESlateVisibility::HitTestInvisible);

	UpdateConfirmButton();

	return FReply::Handled();
}

void UJoustAttackWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(Button_NormalAttack))
	{
		Button_NormalAttack->OnClicked.AddUniqueDynamic(this, &UJoustAttackWidget::HandleNormalAttackClicked);
	}

	if (IsValid(Button_StrongAttack))
	{
		Button_StrongAttack->OnClicked.AddUniqueDynamic(this, &UJoustAttackWidget::HandleStrongAttackClicked);
	}

	if (IsValid(Button_TrickAttack))
	{
		Button_TrickAttack->OnClicked.AddUniqueDynamic(this, &UJoustAttackWidget::HandleTrickAttackClicked);
	}

	if (IsValid(Button_SlowAttack))
	{
		Button_SlowAttack->OnClicked.AddUniqueDynamic(this, &UJoustAttackWidget::HandleSlowAttackClicked);
	}

	if (IsValid(Button_Confirm))
	{
		Button_Confirm->OnClicked.AddUniqueDynamic(this, &UJoustAttackWidget::HandleConfirmClicked);
	}

	SetAttackUsageStates(AttackUsageStates);
	ResetAttackSelection();
}

void UJoustAttackWidget::NativeDestruct()
{
	if (IsValid(Button_NormalAttack))
	{
		Button_NormalAttack->OnClicked.RemoveDynamic(this, &UJoustAttackWidget::HandleNormalAttackClicked);
	}

	if (IsValid(Button_StrongAttack))
	{
		Button_StrongAttack->OnClicked.RemoveDynamic(this, &UJoustAttackWidget::HandleStrongAttackClicked);
	}

	if (IsValid(Button_TrickAttack))
	{
		Button_TrickAttack->OnClicked.RemoveDynamic(this, &UJoustAttackWidget::HandleTrickAttackClicked);
	}

	if (IsValid(Button_SlowAttack))
	{
		Button_SlowAttack->OnClicked.RemoveDynamic(this, &UJoustAttackWidget::HandleSlowAttackClicked);
	}

	if (IsValid(Button_Confirm))
	{
		Button_Confirm->OnClicked.RemoveDynamic(this, &UJoustAttackWidget::HandleConfirmClicked);
	}

	Super::NativeDestruct();
}

