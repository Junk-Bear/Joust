// Fill out your copyright notice in the Description page of Project Settings.


#include "Presentation/JoustMatchResultWidget.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"

void UJoustMatchResultWidget::SetMatchResult(const FJoustMatchResult& InMatchResult, bool bInLocalPlayerA)
{
	if (!IsValid(Border_MatchResultPanel) || !IsValid(Text_MatchOutcome) || !IsValid(Text_PlayerAScore) || !IsValid(Text_PlayerBScore))
		return;
	
	Border_MatchResultPanel->SetVisibility(ESlateVisibility::Visible);

	Text_MatchOutcome->SetText(GetLocalOutcomeText(InMatchResult.MatchOutcome, bInLocalPlayerA));

	Text_PlayerAScore->SetText(FText::AsNumber(InMatchResult.PlayerAScore));

	Text_PlayerBScore->SetText(FText::AsNumber(InMatchResult.PlayerBScore));
}

FText UJoustMatchResultWidget::GetLocalOutcomeText(EJoustMatchOutcome InMatchOutcome, bool bInLocalPlayerA) const
{
	switch (InMatchOutcome)
	{
	case EJoustMatchOutcome::PlayerAWin:
		if (bInLocalPlayerA)
		{
			return FText::FromString(TEXT("VICTORY"));
		}

		return FText::FromString(TEXT("DEFEAT"));

	case EJoustMatchOutcome::PlayerBWin:
		if (bInLocalPlayerA)
		{
			return FText::FromString(TEXT("DEFEAT"));
		}

		return FText::FromString(TEXT("VICTORY"));

	case EJoustMatchOutcome::Draw:
		return FText::FromString(TEXT("DRAW"));

	default:
		return FText::FromString(TEXT("UNDECIDED"));
	}
}
