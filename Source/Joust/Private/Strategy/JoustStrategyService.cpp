// Fill out your copyright notice in the Description page of Project Settings.


#include "Strategy/JoustStrategyService.h"
#include "Rules/JoustRuleSetDataAsset.h"
#include "Interface/JoustRandomProvider.h"
#include "Strategy/FJoustCardDrawService.h"
#include "Strategy/FJoustCardEffectResolver.h"
#include "Strategy/JoustStrategyCardDataAsset.h"

void UJoustStrategyService::Initialize(UJoustRuleSetDataAsset* InRuleSet, IJoustRandomProvider& InRandomProvider)
{
	RuleSet = InRuleSet;
	RandomProvider = &InRandomProvider;

	ResetRoundState();
}

/** 새 전략 라운드 준비 및 초기화 */
bool UJoustStrategyService::PrepareRound(
	const FJoustPlayerStats & InPlayerABaseStats, 
	const FJoustPlayerStats & InPlayerBBaseStats, 
	bool bPlayerAHasBanRight, 
	bool bPlayerBHasBanRight)
{
	ResetRoundState();

	if (RuleSet == nullptr)
		return false;

	if (RuleSet->CardShownPerRound <= 0)
		return false;

	if (!FJoustCardDrawService::Draw(RuleSet->StrategyCardPool,	RuleSet->CardShownPerRound,	*RandomProvider, PublicCards))
		return false;

	PlayerABaseStats = InPlayerABaseStats;
	PlayerBBaseStats = InPlayerBBaseStats;

	PlayerACurrentStats = PlayerABaseStats;
	PlayerBCurrentStats = PlayerBBaseStats;

	const bool bBanSystemEnable = RuleSet->MaxCardBansPerPlayer > 0;

	CardBanService.BeginRound((bBanSystemEnable && bPlayerAHasBanRight), (bBanSystemEnable && bPlayerBHasBanRight));

	bRoundPrepared = true;

	return true;

}

void UJoustStrategyService::EndRound()
{
	ResetRoundState();
}

bool UJoustStrategyService::SubmitBan(bool bPlayerA, FName CardID)
{
	if (!bRoundPrepared || bStrategyFinalized)
		return false;

	return CardBanService.SubmitBan(bPlayerA, CardID, PublicCards);
}

bool UJoustStrategyService::SubmitStrategySelection(bool bPlayerA, FName CardID)
{
	if (!bRoundPrepared || bStrategyFinalized || CardID.IsNone())
		return false;

	if (!CardBanService.AreAllRequiredBanSumitted())
		return false;

	if (bPlayerA)
	{
		if (bPlayerASelectionSubmitted)
			return false;
	}
	else
	{
		if (bPlayerBSelectionSubmitted)
			return false;
	}

	if (CardBanService.IsCardBannedForPlayer(bPlayerA, CardID))
		return false;

	UJoustStrategyCardDataAsset* SelectedCard = FindPublicCardByID(CardID);

	if (SelectedCard == nullptr)
		return false;

	if (bPlayerA)
	{
		PlayerASelectedCard = SelectedCard;
		bPlayerASelectionSubmitted = true;
	}
	else
	{
		PlayerBSelectedCard= SelectedCard;
		bPlayerBSelectionSubmitted = true;
	}

	return true;
}

bool UJoustStrategyService::FinalizeStrategy()
{
	if (!bRoundPrepared || bStrategyFinalized)
		return false;

	if (!AreBothPlayersComplete())
		return false;

	if (PlayerASelectedCard == nullptr || PlayerBSelectedCard == nullptr)
		return false;

	PlayerACurrentStats = FJoustCardEffectResolver::Resolve(PlayerABaseStats, PlayerASelectedCard->Modifier);
	PlayerBCurrentStats = FJoustCardEffectResolver::Resolve(PlayerBBaseStats, PlayerBSelectedCard->Modifier);

	bStrategyFinalized = true;
	
	return true;
}

bool UJoustStrategyService::IsPlayerComplete(bool bPlayerA) const
{
	if (!bRoundPrepared)
		return false;

	if (CardBanService.HasPendingBan(bPlayerA))
		return false;

	return bPlayerA ? bPlayerASelectionSubmitted : bPlayerBSelectionSubmitted;
}

bool UJoustStrategyService::AreBothPlayersComplete() const
{
	return IsPlayerComplete(true) && IsPlayerComplete(false);
}

bool UJoustStrategyService::AreBansComplete() const
{
	return bRoundPrepared && CardBanService.AreAllRequiredBanSumitted();
}

bool UJoustStrategyService::GetSelectableCards(bool bPlayerA, TArray<TObjectPtr<UJoustStrategyCardDataAsset>>& OutCards) const
{
	OutCards.Reset();

	if (!bRoundPrepared)
		return false;

	if (!CardBanService.AreAllRequiredBanSumitted())
		return false;

	OutCards.Reserve(PublicCards.Num());

	for (UJoustStrategyCardDataAsset* CardItem : PublicCards)
	{
		if (CardItem == nullptr)
			continue;

		if (CardBanService.IsCardBannedForPlayer(bPlayerA, CardItem->CardID))
			continue;

		OutCards.Add(CardItem);
	}

	return OutCards.Num() > 0;
}

FName UJoustStrategyService::GetSelectedCardID(bool bPlayerA) const
{
	const UJoustStrategyCardDataAsset* SelectedCard = bPlayerA ? PlayerASelectedCard.Get() : PlayerBSelectedCard.Get();

	return SelectedCard != nullptr ? SelectedCard->CardID : NAME_None;
}

UJoustStrategyCardDataAsset* UJoustStrategyService::FindPublicCardByID(FName CardID) const
{
	for (UJoustStrategyCardDataAsset* CardItem : PublicCards)
	{
		if (CardItem != nullptr && CardItem->CardID == CardID)
		{
			return CardItem;
		}
	}

	return nullptr;
}

void UJoustStrategyService::ResetRoundState()
{
	PublicCards.Reset();

	PlayerASelectedCard = nullptr;
	PlayerBSelectedCard = nullptr;

	PlayerABaseStats = FJoustPlayerStats{};
	PlayerBBaseStats = FJoustPlayerStats{};

	PlayerACurrentStats = FJoustPlayerStats{};
	PlayerBCurrentStats = FJoustPlayerStats{};

	CardBanService.EndRound();

	bRoundPrepared = false;

	bPlayerASelectionSubmitted = false;
	bPlayerBSelectionSubmitted = false;

	bStrategyFinalized = false;
}
