// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class UJoustStrategyCardDataAsset;

/**
 * 카드 한 장을 밴하는 클래스
 */
class JOUST_API FJoustCardBanService
{
public: // ########## public 함수 블록 ##########

	/** 시작시 초기화 */
	void BeginRound(bool bInPlayerAHasBanRight, bool bInPlayerBHasBanRight);

	/** 종료시 초기화 */
	void EndRound();

	/** 플레이어의 상대 카드 봉인단계 */
	bool SubmitBan(bool bBanningPlayerA, FName CardID, 
		const TArray<TObjectPtr<UJoustStrategyCardDataAsset>>& PublicCards
		);

	/** 봉인권 사용 했는지 확인 */
	bool HasPendingBan(bool bPlayerA) const;

	/** 양쪽 봉인권을 모두 사용했는지 확인 */
	bool AreAllRequiredBanSumitted() const;

	/** 카드가 봉인되었는지 확인 */
	bool IsCardBannedForPlayer(bool bTargetPlayerA, FName CardID) const;

	/** 봉인 적용된 카드 ID를 반환 */
	FName GetBannedCardIDForPlayer(bool bTargetPlayerA) const;

private: // ########## private 함수 블록 ##########

	/** 지정한 카드가 이번 공개 카드 목록에 있는지 확인 */
	static bool ContainsCardID(const TArray<TObjectPtr<UJoustStrategyCardDataAsset>>& PublicCards, FName CardID);

private: // ########## private 변수 블록 ##########

	/** Player들의 카드 봉인권 여부 */
	bool bPlayerAHasBanRight = false;
	bool bPlayerBHasBanRight = false;

	/** 봉인권을 사용했는지 여부 */
	bool bPlayerABanSumitted = false;
	bool bPlayerBBanSumitted = false;

	/** 플레이어가 봉인한 카드 */
	FName AToBBannedCard = NAME_None;
	FName BToABannedCard = NAME_None;
};
