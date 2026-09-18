// Fill out your copyright notice in the Description page of Project Settings.


#include "Presentation/JoustDefenseHistoryWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

void UJoustDefenseHistoryWidget::HandleCloseClicked()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UJoustDefenseHistoryWidget::ClearHistory()
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

FVector2D UJoustDefenseHistoryWidget::ConvertPointToCanvasPosition(const FVector2D& InPoint, const FVector2D& InLanceBoxMin, const FVector2D& InLanceBoxMax) const
{
	const float NormalizedX = FMath::GetMappedRangeValueClamped(FVector2D(InLanceBoxMin.X, InLanceBoxMax.X), FVector2D(0.0f, 1.0f), InPoint.X);

	const float NormalizedY = FMath::GetMappedRangeValueClamped(FVector2D(InLanceBoxMin.Y, InLanceBoxMax.Y), FVector2D(0.0f, 1.0f), InPoint.Y);

	return FVector2D(NormalizedX * MarkerCanvasSize, (1.0f - NormalizedY) * MarkerCanvasSize);
}

FLinearColor UJoustDefenseHistoryWidget::ResolveAttackTypeColor(EJoustAttackType InAttackType) const
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

FString UJoustDefenseHistoryWidget::ResolveAttackTypeText(EJoustAttackType InAttackType) const
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

FString UJoustDefenseHistoryWidget::ResolveDefenseMarkerShape(const FJoustDefenseResult& InDefenseResult) const
{
	if (InDefenseResult.bIsEdgeParry)
	{
		return TEXT("★");
	}

	switch (InDefenseResult.ParryOutcome)
	{
	case EJoustParryOutcome::Success:
		return TEXT("◎");

	case EJoustParryOutcome::Failure:
		return TEXT("◇");

	case EJoustParryOutcome::NotAttempted:
	default:
		return TEXT("○");
	}
}

FString UJoustDefenseHistoryWidget::ResolveDefenseResultText(const FJoustDefenseResult& InDefenseResult) const
{
	if (InDefenseResult.bIsEdgeParry)
	{
		return TEXT("엣지 패링");
	}

	switch (InDefenseResult.ParryOutcome)
	{
	case EJoustParryOutcome::Success:
		return TEXT("패링");

	case EJoustParryOutcome::Failure:
		return TEXT("패링 실패");

	case EJoustParryOutcome::NotAttempted:
	default:
		return TEXT("가드");
	}
}

void UJoustDefenseHistoryWidget::AddMarker(
	const FVector2D& InPosition,
	const FString& InLabel,
	const FLinearColor& InColor,
	const FString& InShape,
	bool bInOverlapMarker)
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

	MarkerShapeTextPtr->SetText(FText::FromString(InShape));

	MarkerShapeTextPtr->SetColorAndOpacity(FSlateColor(InColor));

	MarkerShapeTextPtr->SetJustification(ETextJustify::Center);

	FSlateFontInfo ShapeFont = MarkerShapeTextPtr->GetFont();

	ShapeFont.Size = 36;

	MarkerShapeTextPtr->SetFont(ShapeFont);

	MarkerValueTextPtr->SetText(FText::FromString(InLabel));

	MarkerValueTextPtr->SetColorAndOpacity(FSlateColor(FLinearColor::White));

	MarkerValueTextPtr->SetJustification(ETextJustify::Center);

	FSlateFontInfo ValueFont = MarkerValueTextPtr->GetFont();

	ValueFont.Size = 15;

	if (bInOverlapMarker)
	{
		ValueFont.Size = 18;
	}

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

void UJoustDefenseHistoryWidget::AddOverlapDescription(const FString& InDescription)
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

void UJoustDefenseHistoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(Button_Close))
	{
		Button_Close->OnClicked.AddUniqueDynamic(this, &UJoustDefenseHistoryWidget::HandleCloseClicked);
	}
}

void UJoustDefenseHistoryWidget::NativeDestruct()
{
	if (IsValid(Button_Close))
	{
		Button_Close->OnClicked.RemoveDynamic(this, &UJoustDefenseHistoryWidget::HandleCloseClicked);
	}

	ClearHistory();

	Super::NativeDestruct();
}

void UJoustDefenseHistoryWidget::SetHistory(const TArray<FJoustDefenseHistory>& InHistory, const FVector2D& InLanceBoxMin, const FVector2D& InLanceBoxMax)
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
		const FVector2D MarkerPosition = ConvertPointToCanvasPosition(InHistory[i].OpponentAttackPoint, InLanceBoxMin, InLanceBoxMax);

		bool bAddedToGroup = false;

		for (FMarkerGroup& Item : MarkerGroups)
		{
			const bool bOverlapsExistingMarker = Item.HistoryIndices.ContainsByPredicate(
				[this, &InHistory, &InLanceBoxMin, &InLanceBoxMax, &MarkerPosition](int32 InHistoryIndex)
				{
					const FVector2D ExistingPosition = ConvertPointToCanvasPosition(
						InHistory[InHistoryIndex].OpponentAttackPoint,
						InLanceBoxMin,
						InLanceBoxMax);

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
			const FJoustDefenseHistory& HistoryItemRef = InHistory[Item.HistoryIndices[0]];

			const FString HistoryLabel = HistoryItemRef.bUnhorsed ? TEXT("D") : FString::FromInt(HistoryItemRef.RoundNumber);

			const FLinearColor MarkerColor =ResolveAttackTypeColor(HistoryItemRef.OpponentAttackType);

			const FString MarkerShape = ResolveDefenseMarkerShape(HistoryItemRef.DefenseResult);

			AddMarker(Item.MarkerPosition, HistoryLabel, MarkerColor, MarkerShape, false);

			continue;
		}

		++CurrentOverlapNumber;

		const FString OverlapLabel = 
			OverlapGroupCount == 1 ? TEXT("*") : FString::Printf(TEXT("*%d"), CurrentOverlapNumber);

		AddMarker(Item.MarkerPosition, OverlapLabel, FLinearColor::White, TEXT(""), true);

		FString OverlapDescription = OverlapLabel + TEXT(" ");

		for (int32 i = 0; i < Item.HistoryIndices.Num(); ++i)
		{
			const FJoustDefenseHistory& HistoryItemRef = InHistory[Item.HistoryIndices[i]];

			if (i > 0)
			{
				OverlapDescription += TEXT(" / ");
			}

			OverlapDescription += FString::Printf(
				TEXT("R%d %s %s"),
				HistoryItemRef.RoundNumber,
				*ResolveAttackTypeText(HistoryItemRef.OpponentAttackType),
				*ResolveDefenseResultText(HistoryItemRef.DefenseResult));
		}

		AddOverlapDescription(OverlapDescription);
	}
}

