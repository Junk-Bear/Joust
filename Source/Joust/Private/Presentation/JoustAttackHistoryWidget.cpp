// Fill out your copyright notice in the Description page of Project Settings.


#include "Presentation/JoustAttackHistoryWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

void UJoustAttackHistoryWidget::SetHistory(const TArray<FJoustAttackHistory>& InHistory, const FVector2D& InLanceBoxMin, const FVector2D& InLanceBoxMax)
{
	struct FMarkerGroup
	{
		FVector2D MarkerPosition = FVector2D::ZeroVector;

		TArray<int32> HistoryIndices;
	};


	ClearHistory();

	if (!IsValid(Canvas_Markers) || !IsValid(VerticalBox_OverlapDescriptions))
		return;

	if (InLanceBoxMin.X >= InLanceBoxMax.X || InLanceBoxMin.Y >= InLanceBoxMax.Y)
		return;


	TArray<FMarkerGroup> MarkerGroups;

	for (int32 i = 0; i < InHistory.Num(); ++i)
	{
		const FVector2D MarkerPosition = ConvertPointToCanvasPosition(InHistory[i].AttackPoint, InLanceBoxMin, InLanceBoxMax);

		bool bAddedToGroup = false;

		for (FMarkerGroup& Item : MarkerGroups)
		{
			const bool bOverlapsExistingMarker = Item.HistoryIndices.ContainsByPredicate(
					[this, &InHistory, &InLanceBoxMin, &InLanceBoxMax, &MarkerPosition](int32 InHistoryIndex)
				{
					const FVector2D ExistingPosition = ConvertPointToCanvasPosition(InHistory[InHistoryIndex].AttackPoint, InLanceBoxMin, InLanceBoxMax);

					return FVector2D::Distance(MarkerPosition, ExistingPosition) <= OverlapDistance;
				});

			if (bOverlapsExistingMarker)
			{
				Item.HistoryIndices.Add(i);

				bAddedToGroup = true;

				break;
			}
		}


		if (!bAddedToGroup)
		{
			FMarkerGroup& NewGroupRef = MarkerGroups.AddDefaulted_GetRef();

			NewGroupRef.MarkerPosition = MarkerPosition;

			NewGroupRef.HistoryIndices.Add(i);
		}
	}

	int32 OverlapGroupCount = 0;

	for (const FMarkerGroup& Item : MarkerGroups)
	{
		if (Item.HistoryIndices.Num() > 1)
		{
			++OverlapGroupCount;
		}
	}

	int32 CurrentOverlapNumber = 0;

	for (const FMarkerGroup& Item : MarkerGroups)
	{
		if (Item.HistoryIndices.Num() == 1)
		{
			const FJoustAttackHistory& HistoryItemRef = InHistory[Item.HistoryIndices[0]];

			const FString HistoryLabel = HistoryItemRef.bOpponentUnhorsed ? TEXT("D") : FString::FromInt(HistoryItemRef.RoundNumber);

			AddMarker(Item.MarkerPosition, HistoryLabel, ResolveAttackTypeColor(HistoryItemRef.AttackType), false);

			continue;
		}

		++CurrentOverlapNumber;

		const FString OverlapLabel = 
			OverlapGroupCount == 1 ? TEXT("*") : FString::Printf(TEXT("*%d"), CurrentOverlapNumber);

		AddMarker(Item.MarkerPosition, OverlapLabel, FLinearColor::White, true);

		FString OverlapDescription = OverlapLabel + TEXT(" ");

		for (int32 i = 0; i < Item.HistoryIndices.Num(); ++i)
		{
			const FJoustAttackHistory& HistoryItemRef = InHistory[Item.HistoryIndices[i]];

			if (i > 0)
			{
				OverlapDescription += TEXT(" / ");
			}

			OverlapDescription += FString::Printf(TEXT("R%d %s"), HistoryItemRef.RoundNumber, *ResolveAttackTypeText(HistoryItemRef.AttackType));
		}

		AddOverlapDescription(OverlapDescription);
	}
}

void UJoustAttackHistoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(Button_Close))
	{
		Button_Close->OnClicked.AddUniqueDynamic(this, &UJoustAttackHistoryWidget::HandleCloseClicked);
	}
}

void UJoustAttackHistoryWidget::NativeDestruct()
{
	if (IsValid(Button_Close))
	{
		Button_Close->OnClicked.RemoveDynamic(this, &UJoustAttackHistoryWidget::HandleCloseClicked);
	}

	ClearHistory();

	Super::NativeDestruct();
}

void UJoustAttackHistoryWidget::HandleCloseClicked()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UJoustAttackHistoryWidget::ClearHistory()
{
	if (IsValid(Canvas_Markers))
	{
		Canvas_Markers->ClearChildren();
	}

	if (IsValid(VerticalBox_OverlapDescriptions))
	{
		VerticalBox_OverlapDescriptions->ClearChildren();
	}
}

FVector2D UJoustAttackHistoryWidget::ConvertPointToCanvasPosition(const FVector2D & InPoint, const FVector2D & InLanceBoxMin, const FVector2D & InLanceBoxMax) const
{
	const float NormalizedX = FMath::GetMappedRangeValueClamped(FVector2D(InLanceBoxMin.X, InLanceBoxMax.X), FVector2D(0.0f, 1.0f), InPoint.X);

	const float NormalizedY = FMath::GetMappedRangeValueClamped(FVector2D(InLanceBoxMin.Y, InLanceBoxMax.Y), FVector2D(0.0f, 1.0f), InPoint.Y);

	return FVector2D(NormalizedX * MarkerCanvasSize, (1.0f - NormalizedY) * MarkerCanvasSize);
}

FLinearColor UJoustAttackHistoryWidget::ResolveAttackTypeColor(EJoustAttackType InAttackType) const
{
	switch (InAttackType)
	{
	case EJoustAttackType::Strong:
		return FLinearColor::Red; 

	case EJoustAttackType::Trick:
		return FLinearColor::Green;

	case EJoustAttackType::Slow:
		return FLinearColor::Blue;

	case EJoustAttackType::Normal:
	default:
		return FLinearColor::White;
	}
}

FString UJoustAttackHistoryWidget::ResolveAttackTypeText(EJoustAttackType InAttackType) const
{
	switch (InAttackType)
	{
	case EJoustAttackType::Strong:
		return TEXT("강공");

	case EJoustAttackType::Trick:
		return TEXT("트릭");

	case EJoustAttackType::Slow:
		return TEXT("느림");

	case EJoustAttackType::Normal:
	default:
		return TEXT("일반");
	}
}

void UJoustAttackHistoryWidget::AddMarker(const FVector2D& InPosition, const FString& InLabel, const FLinearColor& InColor, bool bInOverlapMarker)
{
	if (!IsValid(Canvas_Markers) || !IsValid(WidgetTree))
		return;

	UOverlay* MarkerOverlayPtr = WidgetTree->ConstructWidget<UOverlay>();

	if (!IsValid(MarkerOverlayPtr))
		return;

	MarkerOverlayPtr->SetVisibility(ESlateVisibility::HitTestInvisible);

	UCanvasPanelSlot* CanvasSlotPtr = Canvas_Markers->AddChildToCanvas(MarkerOverlayPtr);

	if (!IsValid(CanvasSlotPtr))
		return;

	CanvasSlotPtr->SetPosition(InPosition);

	CanvasSlotPtr->SetSize(FVector2D(MarkerSize, MarkerSize));

	CanvasSlotPtr->SetAlignment(FVector2D(0.5f, 0.5f));

	CanvasSlotPtr->SetAutoSize(false);

	UTextBlock* MarkerShapeTextPtr = WidgetTree->ConstructWidget<UTextBlock>();
	UTextBlock* MarkerValueTextPtr = WidgetTree->ConstructWidget<UTextBlock>();

	if (!IsValid(MarkerShapeTextPtr) || !IsValid(MarkerValueTextPtr))
		return;

	MarkerShapeTextPtr->SetText(bInOverlapMarker ? FText::GetEmpty() : FText::FromString(TEXT("●")));

	MarkerShapeTextPtr->SetColorAndOpacity(FSlateColor(InColor));

	MarkerShapeTextPtr->SetJustification(ETextJustify::Center);

	FSlateFontInfo ShapeFont = MarkerShapeTextPtr->GetFont();

	ShapeFont.Size = 34;

	MarkerShapeTextPtr->SetFont(ShapeFont);

	MarkerValueTextPtr->SetText(FText::FromString(InLabel));

	MarkerValueTextPtr->SetJustification(ETextJustify::Center);

	const float MarkerBrightness =
		InColor.R * 0.299f +
		InColor.G * 0.587f +
		InColor.B * 0.114f;

	MarkerValueTextPtr->SetColorAndOpacity(FSlateColor(bInOverlapMarker ? FLinearColor::White : 
		( MarkerBrightness > 0.5f? FLinearColor::Black : FLinearColor::White )));

	FSlateFontInfo ValueFont = MarkerValueTextPtr->GetFont();
	ValueFont.Size = bInOverlapMarker ? 18 : 13;
	MarkerValueTextPtr->SetFont(ValueFont);

	UOverlaySlot* ShapeSlotPtr = MarkerOverlayPtr->AddChildToOverlay(MarkerShapeTextPtr);

	UOverlaySlot* ValueSlotPtr = MarkerOverlayPtr->AddChildToOverlay(MarkerValueTextPtr);

	if (IsValid(ShapeSlotPtr))
	{
		ShapeSlotPtr->SetHorizontalAlignment(HAlign_Fill);

		ShapeSlotPtr->SetVerticalAlignment(VAlign_Fill);
	}

	if (IsValid(ValueSlotPtr))
	{
		ValueSlotPtr->SetHorizontalAlignment(HAlign_Fill);

		ValueSlotPtr->SetVerticalAlignment(VAlign_Center);
	}
}

void UJoustAttackHistoryWidget::AddOverlapDescription(const FString & InDescription)
{
	if (!IsValid(VerticalBox_OverlapDescriptions) || !IsValid(WidgetTree))
		return;

	UTextBlock* DescriptionTextPtr = WidgetTree->ConstructWidget<UTextBlock>();

	if (!IsValid(DescriptionTextPtr))
		return;

	DescriptionTextPtr->SetText(FText::FromString(InDescription));

	DescriptionTextPtr->SetColorAndOpacity(FSlateColor(FLinearColor::White));

	FSlateFontInfo DescriptionFont = DescriptionTextPtr->GetFont();

	DescriptionFont.Size = 14;

	DescriptionTextPtr->SetFont(DescriptionFont);

	VerticalBox_OverlapDescriptions->AddChildToVerticalBox(DescriptionTextPtr);
}
