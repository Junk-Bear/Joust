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
	bool bInPlayerAHasBanRight, 
	bool bInPlayerBHasBanRight)
{
	ResetRoundState();

	if (!IsValid(RuleSet))
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

	CardBanService.BeginRound((bBanSystemEnable && bInPlayerAHasBanRight), (bBanSystemEnable && bInPlayerBHasBanRight));

	bRoundPrepared = true;

	return true;

}

void UJoustStrategyService::EndRound()
{
	ResetRoundState();
}

bool UJoustStrategyService::SubmitBan(bool bInPlayerA, FName InCardID)
{
	if (!bRoundPrepared || bStrategyFinalized)
		return false;

	return CardBanService.SubmitBan(bInPlayerA, InCardID, PublicCards);
}

bool UJoustStrategyService::SubmitStrategySelection(bool bInPlayerA, FName InCardID)
{
	if (!bRoundPrepared || bStrategyFinalized || InCardID.IsNone())
		return false;

	if (!CardBanService.AreAllRequiredBanSumitted())
		return false;

	if (bInPlayerA)
	{
		if (bPlayerASelectionSubmitted)
			return false;
	}
	else
	{
		if (bPlayerBSelectionSubmitted)
			return false;
	}

	if (CardBanService.IsCardBannedForPlayer(bInPlayerA, InCardID))
		return false;

	UJoustStrategyCardDataAsset* SelectedCardPtr = FindPublicCardByID(InCardID);

	if (!IsValid(SelectedCardPtr))
		return false;

	if (bInPlayerA)
	{
		PlayerASelectedCard = SelectedCardPtr;
		bPlayerASelectionSubmitted = true;
	}
	else
	{
		PlayerBSelectedCard= SelectedCardPtr;
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

	if (!IsValid(PlayerASelectedCard) || !IsValid(PlayerBSelectedCard))
		return false;

	PlayerACurrentStats = FJoustCardEffectResolver::Resolve(PlayerABaseStats, PlayerASelectedCard->Modifier);
	PlayerBCurrentStats = FJoustCardEffectResolver::Resolve(PlayerBBaseStats, PlayerBSelectedCard->Modifier);

	bStrategyFinalized = true;
	
	return true;
}

bool UJoustStrategyService::IsPlayerComplete(bool bInPlayerA) const
{
	if (!bRoundPrepared)
		return false;

	if (CardBanService.HasPendingBan(bInPlayerA))
		return false;

	return bInPlayerA ? bPlayerASelectionSubmitted : bPlayerBSelectionSubmitted;
}

bool UJoustStrategyService::AreBothPlayersComplete() const
{
	return IsPlayerComplete(true) && IsPlayerComplete(false);
}

bool UJoustStrategyService::AreBansComplete() const
{
	return bRoundPrepared && CardBanService.AreAllRequiredBanSumitted();
}

bool UJoustStrategyService::GetSelectableCards(bool bInPlayerA, TArray<TObjectPtr<UJoustStrategyCardDataAsset>>& OutCards) const
{
	OutCards.Reset();

	if (!bRoundPrepared)
		return false;

	if (!CardBanService.AreAllRequiredBanSumitted())
		return false;

	OutCards.Reserve(PublicCards.Num());

	for (UJoustStrategyCardDataAsset* Item : PublicCards)
	{
		if (!IsValid(Item))
			continue;

		if (CardBanService.IsCardBannedForPlayer(bInPlayerA, Item->CardID))
			continue;

		OutCards.Add(Item);
	}

	return OutCards.Num() > 0;
}

FName UJoustStrategyService::GetSelectedCardID(bool bInPlayerA) const
{
	const UJoustStrategyCardDataAsset* SelectedCardPtr = bInPlayerA ? PlayerASelectedCard.Get() : PlayerBSelectedCard.Get();

	return IsValid(SelectedCardPtr) ? SelectedCardPtr->CardID : NAME_None;
}

UJoustStrategyCardDataAsset* UJoustStrategyService::FindPublicCardByID(FName InCardID) const
{
	for (UJoustStrategyCardDataAsset* Item : PublicCards)
	{
		if (IsValid(Item) && Item->CardID == InCardID)
		{
			return Item;
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
