// Fill out your copyright notice in the Description page of Project Settings.


#include "Presentation/JoustRoundResultWidget.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"


void UJoustRoundResultWidget::SetRoundResult(const FJoustRoundResult& InRoundResult, bool bInLocalPlayerA, int32 InPlayerAScore, int32 InPlayerBScore)
{
	if (!IsValid(Border_ResultPanel) ||
		!IsValid(Text_RoundTitle) ||
		!IsValid(Text_AttackValue) ||
		!IsValid(Text_DefenseValue) ||
		!IsValid(Text_ScoreValue) ||
		!IsValid(Text_TotalValue) ||
		!IsValid(Text_Unhorse)
		)
		return;
	
	const FJoustExchangeResult& LocalAttackExchangeRef = bInLocalPlayerA ? InRoundResult.AtoBExchangeResult : InRoundResult.BtoAExchangeResult;
	const FJoustExchangeResult& LocalDefenseExchangeRef = bInLocalPlayerA ? InRoundResult.BtoAExchangeResult : InRoundResult.AtoBExchangeResult;

	const bool bLocalPlayerUnhorsed = bInLocalPlayerA ? InRoundResult.BtoAExchangeResult.bDefenderUnhorsed : InRoundResult.AtoBExchangeResult.bDefenderUnhorsed;

	const bool bOpponentUnhorsed = bInLocalPlayerA ? InRoundResult.AtoBExchangeResult.bDefenderUnhorsed : InRoundResult.BtoAExchangeResult.bDefenderUnhorsed;
	if (bLocalPlayerUnhorsed || bOpponentUnhorsed)
	{
		Border_ResultPanel->SetVisibility(ESlateVisibility::Collapsed);

		Text_Unhorse->SetVisibility(ESlateVisibility::Visible);

		if (bLocalPlayerUnhorsed && bOpponentUnhorsed)
		{
			Text_Unhorse->SetText(FText::FromString(TEXT("BOTH PLAYER UNHORSED")));
		}
		else if (bLocalPlayerUnhorsed)
		{
			Text_Unhorse->SetText(FText::FromString(TEXT("YOU WERE UNHORSED")));
		}
		else
		{
			Text_Unhorse->SetText(FText::FromString(TEXT("OPPONENT UNHORSED")));
		}

		return;
	}

	Border_ResultPanel->SetVisibility(ESlateVisibility::Visible);

	Text_Unhorse->SetVisibility(ESlateVisibility::Collapsed);

	Text_RoundTitle->SetText(FText::Format(NSLOCTEXT("JoustRoundResultWidget", "RoundResultTitleFormat", "ROUND {0} RESULT"), FText::AsNumber(InRoundResult.RoundNumber)));

	Text_AttackValue->SetText(GetAttackResultText(LocalAttackExchangeRef));

	Text_DefenseValue->SetText(GetDefenseResultText(LocalDefenseExchangeRef));

	Text_ScoreValue->SetText(FText::Format(NSLOCTEXT("JoustRoundResultWidget", "RoundScoreFormat", "+{0} POINT"), FText::AsNumber(LocalAttackExchangeRef.ScoreDelta)));

	Text_TotalValue->SetText(FText::Format(NSLOCTEXT("JoustRoundResultWidget", "TotalScoreFormat", "{0} : {1}"), FText::AsNumber(InPlayerAScore), FText::AsNumber(InPlayerBScore)));
}

FText UJoustRoundResultWidget::GetAttackResultText(const FJoustExchangeResult& InExchangeResult) const
{
	if (InExchangeResult.bDefenderUnhorsed)
	{
		return FText::FromString(TEXT("UNHORSED"));
	}

	if (InExchangeResult.ScoreDelta > 0)
	{
		return FText::FromString(TEXT("HIT"));
	}

	if (InExchangeResult.DefenseResult.bBlockedScore)
	{
		return FText::FromString(TEXT("BLOCKED"));
	}

	return FText::FromString(TEXT("NO SCORE"));
}

FText UJoustRoundResultWidget::GetDefenseResultText(const FJoustExchangeResult& InExchangeResult) const
{
	if (InExchangeResult.bDefenderUnhorsed)
	{
		return FText::FromString(TEXT("UNHORSED"));
	}

	if (InExchangeResult.DefenseResult.bIsEdgeParry)
	{
		return FText::FromString(TEXT("EDGE PARRY"));
	}

	if (InExchangeResult.DefenseResult.ParryOutcome == EJoustParryOutcome::Success)
	{
		return FText::FromString(TEXT("PARRY"));
	}

	if (InExchangeResult.DefenseResult.bBlockedScore)
	{
		return FText::FromString(TEXT("GUARD"));
	}

	return FText::FromString(TEXT("HIT"));
}
