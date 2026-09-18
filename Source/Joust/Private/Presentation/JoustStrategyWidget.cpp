// Fill out your copyright notice in the Description page of Project Settings.


#include "Presentation/JoustStrategyWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Rules/JoustRuleSetDataAsset.h"
#include "Strategy/JoustStrategyCardDataAsset.h"
#include "Presentation/JoustAttackHistoryWidget.h"
#include "Presentation/JoustDefenseHistoryWidget.h"
#include "Components/Overlay.h"

void UJoustStrategyWidget::SetStrategyCards(const TArray<FName>& InCardIDs, const UJoustRuleSetDataAsset* InRuleSet)
{
	DisplayedCardIDs.Init(NAME_None, CardButtons.Num());

	for (int32 i = 0; i < CardButtons.Num(); ++i)
	{
		UButton* CardButtonPtr = CardButtons[i];
		UTextBlock* CardNameTextPtr = CardNameTexts[i];
		UTextBlock* CardEffectTextPtr = CardEffectTexts[i];

		const UJoustStrategyCardDataAsset* CardDataPtr = nullptr;
		if (IsValid(InRuleSet) && InCardIDs.IsValidIndex(i) && !InCardIDs[i].IsNone())
		{
			for (const UJoustStrategyCardDataAsset* Item : InRuleSet->StrategyCardPool)
			{
				if (IsValid(Item) && Item->CardID == InCardIDs[i])
				{
					CardDataPtr = Item;

					break;
				}
			}
		}

		if (IsValid(CardDataPtr))
		{
			DisplayedCardIDs[i] = CardDataPtr->CardID;

			CardNameTextPtr->SetText(CardDataPtr->DisplayName);
			CardEffectTextPtr->SetText(CardDataPtr->Description);

			CardButtonPtr->SetIsEnabled(true);
			CardButtonPtr->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			CardNameTextPtr->SetText(FText::GetEmpty());
			CardEffectTextPtr->SetText(FText::GetEmpty());

			CardButtonPtr->SetIsEnabled(false);
			CardButtonPtr->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	ResetCardSelection();
}

void UJoustStrategyWidget::ResetCardSelection()
{
	SelectedCardIdx = INDEX_NONE;

	Button_StrategyConfirm->SetIsEnabled(false);

	UpdateCardSelectionVisuals();
}

void UJoustStrategyWidget::SetStrategyInteractionState(bool bInLocalBanPending, bool bInAnyBanPending, FName InBannedCardIDForPlayer)
{
	const EJoustStrategyInteractionMode PreviousInteractionMode = InteractionMode;

	BannedCardIDForPlayer = InBannedCardIDForPlayer;

	if (bInLocalBanPending)
	{
		InteractionMode = EJoustStrategyInteractionMode::Ban;
	}
	else if (bInAnyBanPending)
	{
		InteractionMode = EJoustStrategyInteractionMode::Waiting;
	}
	else
	{
		InteractionMode = EJoustStrategyInteractionMode::Selection;
	}

	if (InteractionMode != PreviousInteractionMode)
	{
		ResetCardSelection();

		CloseHistoryPopups();
	}



	RefreshInteractionState();
}

void UJoustStrategyWidget::SetHistoryData(const TArray<FJoustAttackHistory>& InAttackHistory, const TArray<FJoustDefenseHistory>& InDefenseHistory, const FVector2D& InLanceBoxMin, const FVector2D& InLanceBoxMax)
{
	if (IsValid(WBP_JoustAttackHistory))
	{
		WBP_JoustAttackHistory->SetHistory(InAttackHistory, InLanceBoxMin, InLanceBoxMax);
	}

	if (IsValid(WBP_JoustDefenseHistory))
	{
		WBP_JoustDefenseHistory->SetHistory(InDefenseHistory, InLanceBoxMin, InLanceBoxMax);
	}
}

void UJoustStrategyWidget::ResetStrategyScreen()
{
	ResetCardSelection();

	CloseHistoryPopups();
}

void UJoustStrategyWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	CardButtons = { Button_StrategyCard1, Button_StrategyCard2, Button_StrategyCard3, Button_StrategyCard4, Button_StrategyCard5 };

	CardNameTexts = { Text_Card1Name, Text_Card2Name, Text_Card3Name, Text_Card4Name, Text_Card5Name };

	CardEffectTexts = { Text_Card1Effect, Text_Card2Effect, Text_Card3Effect, Text_Card4Effect, Text_Card5Effect };

	DisplayedCardIDs.Init(NAME_None, CardButtons.Num());
}

void UJoustStrategyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Button_StrategyCard1->OnClicked.AddUniqueDynamic(this, &UJoustStrategyWidget::HandleStrategyCard1Clicked);

	Button_StrategyCard2->OnClicked.AddUniqueDynamic(this, &UJoustStrategyWidget::HandleStrategyCard2Clicked);

	Button_StrategyCard3->OnClicked.AddUniqueDynamic(this, &UJoustStrategyWidget::HandleStrategyCard3Clicked);

	Button_StrategyCard4->OnClicked.AddUniqueDynamic(this, &UJoustStrategyWidget::HandleStrategyCard4Clicked);

	Button_StrategyCard5->OnClicked.AddUniqueDynamic(this, &UJoustStrategyWidget::HandleStrategyCard5Clicked);

	Button_StrategyConfirm->OnClicked.AddUniqueDynamic(this, &UJoustStrategyWidget::HandleStrategyConfirmClicked);

	Button_AttackHistory->OnClicked.AddUniqueDynamic(this, &UJoustStrategyWidget::HandleAttackHistoryClicked);

	Button_DefenseHistory->OnClicked.AddUniqueDynamic(this, &UJoustStrategyWidget::HandleDefenseHistoryClicked);

	CloseHistoryPopups();
	RefreshInteractionState();
}

void UJoustStrategyWidget::NativeDestruct()
{
	Button_StrategyCard1->OnClicked.RemoveDynamic(this, &UJoustStrategyWidget::HandleStrategyCard1Clicked);

	Button_StrategyCard2->OnClicked.RemoveDynamic(this, &UJoustStrategyWidget::HandleStrategyCard2Clicked);

	Button_StrategyCard3->OnClicked.RemoveDynamic(this, &UJoustStrategyWidget::HandleStrategyCard3Clicked);

	Button_StrategyCard4->OnClicked.RemoveDynamic(this, &UJoustStrategyWidget::HandleStrategyCard4Clicked);

	Button_StrategyCard5->OnClicked.RemoveDynamic(this, &UJoustStrategyWidget::HandleStrategyCard5Clicked);

	Button_StrategyConfirm->OnClicked.RemoveDynamic(this, &UJoustStrategyWidget::HandleStrategyConfirmClicked);

	Button_AttackHistory->OnClicked.RemoveDynamic(this, &UJoustStrategyWidget::HandleAttackHistoryClicked);

	Button_DefenseHistory->OnClicked.RemoveDynamic(this, &UJoustStrategyWidget::HandleDefenseHistoryClicked);

	Super::NativeDestruct();
}

void UJoustStrategyWidget::HandleStrategyCard1Clicked()
{
	HandleStrategyCardClicked(0);
}

void UJoustStrategyWidget::HandleStrategyCard2Clicked()
{
	HandleStrategyCardClicked(1);
}

void UJoustStrategyWidget::HandleStrategyCard3Clicked()
{
	HandleStrategyCardClicked(2);
}

void UJoustStrategyWidget::HandleStrategyCard4Clicked()
{
	HandleStrategyCardClicked(3);
}

void UJoustStrategyWidget::HandleStrategyCard5Clicked()
{
	HandleStrategyCardClicked(4);
}

void UJoustStrategyWidget::HandleStrategyCardClicked(int32 InSlotIdx)
{
	if (!IsCardSelectable(InSlotIdx))
		return;

	SelectedCardIdx = InSlotIdx;

	UpdateCardSelectionVisuals();

	Button_StrategyConfirm->SetIsEnabled(true);
}

void UJoustStrategyWidget::UpdateCardSelectionVisuals()
{
	for (int32 i = 0; i < CardButtons.Num(); ++i)
	{
		UButton* CardButtonPtr = CardButtons[i];
		if (!IsValid(CardButtonPtr))
			continue;

		CardButtonPtr->SetBackgroundColor(i == SelectedCardIdx ? SelectedCardColor : NormalCardColor);
	}
}

void UJoustStrategyWidget::HandleStrategyConfirmClicked()
{
	if (!IsCardSelectable(SelectedCardIdx))
		return;
	
	const FName SelectedCardID = DisplayedCardIDs[SelectedCardIdx];

	for (UButton* Item : CardButtons)
	{
		if (IsValid(Item))
		{
			Item->SetIsEnabled(false);
		}
	}

	Button_StrategyConfirm->SetIsEnabled(false);

	if (InteractionMode == EJoustStrategyInteractionMode::Ban)
	{
		StrategyBanConfirmedEvent.Broadcast(SelectedCardID);

		return;
	}

	if (InteractionMode == EJoustStrategyInteractionMode::Selection)
	{
		StrategyConfirmedEvent.Broadcast(SelectedCardID);
	}
}

void UJoustStrategyWidget::RefreshInteractionState()
{
	const bool bWaiting = (InteractionMode == EJoustStrategyInteractionMode::Waiting);

	Overlay_BanWaiting->SetVisibility(bWaiting ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

	switch (InteractionMode)
	{
	case EJoustStrategyInteractionMode::Ban:
		Text_StrategyInstruction->SetText(FText::FromString(TEXT("SELECT A CARD TO BAN")));

		break;

	case EJoustStrategyInteractionMode::Waiting:
		Text_StrategyInstruction->SetText(FText::FromString(TEXT("WAITING FOR OPPONENT")));

		Text_BanWaitingMessage->SetText(FText::FromString(TEXT("WAITING FOR OPPONENT BAN")));

		break;

	case EJoustStrategyInteractionMode::Selection:
	default:
		Text_StrategyInstruction->SetText(FText::FromString(TEXT("SELECT YOUR STRATEGY")));

		break;
	}

	for (int32 i = 0; i < CardButtons.Num(); ++i)
	{
		if (IsValid(CardButtons[i]))
		{
			CardButtons[i]->SetIsEnabled(IsCardSelectable(i));
		}
	}

	Button_StrategyConfirm->SetIsEnabled(IsCardSelectable(SelectedCardIdx));

	const bool bHistoryAvailable = (InteractionMode == EJoustStrategyInteractionMode::Selection);

	Button_AttackHistory->SetIsEnabled(bHistoryAvailable);

	Button_DefenseHistory->SetIsEnabled(bHistoryAvailable);
}

bool UJoustStrategyWidget::IsCardSelectable(int32 InCardIdx) const
{
	if (!CardButtons.IsValidIndex(InCardIdx) || !DisplayedCardIDs.IsValidIndex(InCardIdx) || !IsValid(CardButtons[InCardIdx]) || DisplayedCardIDs[InCardIdx].IsNone())
		return false;
	
	if (InteractionMode == EJoustStrategyInteractionMode::Waiting)
		return false;
	
	if (InteractionMode == EJoustStrategyInteractionMode::Selection && DisplayedCardIDs[InCardIdx] == BannedCardIDForPlayer)
		return false;
	
	return true;
}

void UJoustStrategyWidget::HandleAttackHistoryClicked()
{
	WBP_JoustDefenseHistory->SetVisibility(ESlateVisibility::Collapsed);

	WBP_JoustAttackHistory->SetVisibility(ESlateVisibility::Visible);
}

void UJoustStrategyWidget::HandleDefenseHistoryClicked()
{
	WBP_JoustAttackHistory->SetVisibility(ESlateVisibility::Collapsed);

	WBP_JoustDefenseHistory->SetVisibility(ESlateVisibility::Visible);
}

void UJoustStrategyWidget::CloseHistoryPopups()
{
	WBP_JoustAttackHistory->SetVisibility(ESlateVisibility::Collapsed);

	WBP_JoustDefenseHistory->SetVisibility(ESlateVisibility::Collapsed);
}
