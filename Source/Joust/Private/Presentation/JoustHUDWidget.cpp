// Fill out your copyright notice in the Description page of Project Settings.


#include "Presentation/JoustHUDWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Border.h"
#include "Presentation/JoustStrategyWidget.h"
#include "Presentation/JoustAttackWidget.h"
#include "Presentation/JoustDefenseWidget.h"
#include "Presentation/JoustRoundResultWidget.h"
#include "Presentation/JoustMatchResultWidget.h"
#include "Components/TextBlock.h"
#include "Presentation/JoustMatchStartWidget.h"

void UJoustHUDWidget::ShowMatchStart()
{
	Switcher_Phase->SetActiveWidget(WBP_JoustMatchStart);
	Border_CommonHUD->SetVisibility(ESlateVisibility::Collapsed);
}

void UJoustHUDWidget::ShowPhase(EJoustPhase InPhase, bool bInUnhorseResult)
{
	switch (InPhase)
	{
	case EJoustPhase::Strategy:
		Switcher_Phase->SetActiveWidget(WBP_JoustStrategy);
		Border_CommonHUD->SetVisibility(ESlateVisibility::Visible);
		break;

	case EJoustPhase::Attack:
		Switcher_Phase->SetActiveWidget(WBP_JoustAttack);
		Border_CommonHUD->SetVisibility(ESlateVisibility::Visible);
		break;

	case EJoustPhase::Defense:
		Switcher_Phase->SetActiveWidget(WBP_JoustDefense);
		Border_CommonHUD->SetVisibility(ESlateVisibility::Visible);
		break;

	case EJoustPhase::Resolve:
		Switcher_Phase->SetActiveWidget(WBP_JoustDefense);
		Border_CommonHUD->SetVisibility(ESlateVisibility::Visible);
		break;

	case EJoustPhase::RoundResult:
		Switcher_Phase->SetActiveWidget(WBP_JoustRoundResult);
		Border_CommonHUD->SetVisibility(bInUnhorseResult ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
		break;

	case EJoustPhase::MatchResult:
		Switcher_Phase->SetActiveWidget(WBP_JoustMatchResult);
		Border_CommonHUD->SetVisibility(ESlateVisibility::Collapsed);
		break;
	}
}

void UJoustHUDWidget::UpdateCommonHUD(int32 InRoundNumber, int32 InPlayerAScore, int32 InPlayerBScore, EJoustPhase InPhase)
{
	Text_PlayerAScore->SetText(FText::Format(NSLOCTEXT("JoustHUDWidget", "PlayerAScoreFormat", "PLAYER A : {0}"), FText::AsNumber(InPlayerAScore)));

	Text_PlayerBScore->SetText(FText::Format(NSLOCTEXT("JoustHUDWidget", "PlayerBScoreFormat", "PLAYER B : {0}"), FText::AsNumber(InPlayerBScore)));

	Text_Round->SetText(FText::Format(NSLOCTEXT("JoustHUDWidget", "RoundFormat", "ROUND {0}"), FText::AsNumber(InRoundNumber)));

	switch (InPhase)
	{
	case EJoustPhase::Strategy:
		Text_Phase->SetText(NSLOCTEXT("JoustHUDWidget", "StrategyPhase", "STRATEGY"));
		break;

	case EJoustPhase::Attack:
		Text_Phase->SetText(NSLOCTEXT("JoustHUDWidget", "AttackPhase", "ATTACK"));
		break;

	case EJoustPhase::Defense:
		Text_Phase->SetText(NSLOCTEXT("JoustHUDWidget", "DefensePhase", "DEFENSE"));
		break;

	case EJoustPhase::Resolve:
		Text_Phase->SetText(NSLOCTEXT("JoustHUDWidget", "ResolvePhase", "RESOLVE"));
		break;

	case EJoustPhase::RoundResult:
		Text_Phase->SetText(NSLOCTEXT("JoustHUDWidget", "RoundResultPhase", "ROUND RESULT"));
		break;

	case EJoustPhase::MatchResult:
		Text_Phase->SetText(NSLOCTEXT("JoustHUDWidget", "MatchResultPhase", "MATCH RESULT"));
		break;
	}
}

void UJoustHUDWidget::UpdateRemainingTime(int32 InRemainingSeconds)
{
	Text_Timer->SetText(FText::AsNumber(FMath::Max(0, InRemainingSeconds)));
}
