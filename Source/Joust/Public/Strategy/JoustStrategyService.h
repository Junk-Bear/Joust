// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Player/JoustPlayerTypes.h"
#include "Strategy/FJoustCardBanService.h"
#include "JoustStrategyService.generated.h"

class UJoustRuleSetDataAsset;
class IJoustRandomProvider;
class UJoustStrategyCardDataAsset;

/**
 * 
 */
UCLASS()
class JOUST_API UJoustStrategyService : public UObject
{
	GENERATED_BODY()
	
public: // ########## public 함수 블록 ##########

	/** 사용할 RuleSet과 RandomProvider 설정 */
	void Initialize(UJoustRuleSetDataAsset* InRuleSet, IJoustRandomProvider& InRandomProvider);

	/** 새 전략 라운드 준비 및 초기화 */
	bool PrepareRound(
		const FJoustPlayerStats& InPlayerABaseStats,
		const FJoustPlayerStats& InPlayerBBaseStats,
		bool bPlayerAHasBanRight,
		bool bPlayerBHasBanRight
	);

	/** 종료시 모든 상태 초기화 */
	void EndRound();

	/** 공개 카드 봉인 */
	bool SubmitBan(bool bPlayerA, FName CardID);

	/** 전략 카드 제출 */
	bool SubmitStrategySelection(bool bPlayerA, FName CardID);

	/** 전략 카드 효과를 스탯에 적용 */
	bool FinalizeStrategy();

	/** 해당 플레이어의 Strategy 입력이 완료되었는지 확인 */
	bool IsPlayerComplete(bool bPlayerA) const;

	/** 양쪽 플레이어 모두 Strategy 입력이 완료되었는지 확인 */
	bool AreBothPlayersComplete() const;

	/** 양쪽 플레이어 모두 카드 봉인이 완료되었는지 확인 */
	bool AreBansComplete() const;

	/** 해당 플레이어가 현재 선택할 수 있는 카드 목록을 반환 */
	bool GetSelectableCards(bool bPlayerA, TArray<TObjectPtr<UJoustStrategyCardDataAsset>>& OutCards) const;

	/** 해당 플레이어가 선택한 전략 카드 ID 반환 */
	FName GetSelectedCardID(bool bPlayerA) const;

private: // ########## private 함수 블록 ##########

	/** 공개 카드 중 CardID와 일치하는 카드를 찾기 */
	UJoustStrategyCardDataAsset* FindPublicCardByID(FName CardID) const;

	/** RuleSet / RandomProvider는 유지한 채로 라운드 상태만 초기화 */
	void ResetRoundState();

private: // ########### private 변수 블록 ###########

	/** RuleSet */
	UPROPERTY(Transient)
	TObjectPtr<UJoustRuleSetDataAsset> RuleSet = nullptr;

	/** 이번 라운드에 공개된 공통 카드들 */
	UPROPERTY(Transient)
	TArray<TObjectPtr<UJoustStrategyCardDataAsset>> PublicCards;

	/** Player A / B가 이번 라운드에 선택한 카드 */
	UPROPERTY(Transient)
	TObjectPtr<UJoustStrategyCardDataAsset> PlayerASelectedCard = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UJoustStrategyCardDataAsset> PlayerBSelectedCard = nullptr;

	/** 전략 카드 적용 전 기본 능력치 */
	UPROPERTY(Transient)
	FJoustPlayerStats PlayerABaseStats;

	UPROPERTY(Transient)
	FJoustPlayerStats PlayerBBaseStats;

	/** 전략 카드 적용 후 능력치 */
	UPROPERTY(Transient)
	FJoustPlayerStats PlayerACurrentStats;
	
	UPROPERTY(Transient)
	FJoustPlayerStats PlayerBCurrentStats;

	/** 랜덤 프로바이더 */
	IJoustRandomProvider* RandomProvider = nullptr;

	/** 이번 라운드 카드 봉인 상태 관리 */
	FJoustCardBanService CardBanService;

	/** 이번 라운드가 준비 되었는지 */
	bool bRoundPrepared = false;

	/** Player A / B가 카드 선택을 제출했는지 여부 */
	bool bPlayerASelectionSubmitted = false;
	bool bPlayerBSelectionSubmitted = false;

	/** 카드 효과 적용이 완료되었는지 여부 */
	bool bStrategyFinalized = false;

	public: // ########## GET SET 블록 ##########

		FORCEINLINE const TArray<TObjectPtr<UJoustStrategyCardDataAsset>>& GetPublicCards() const { return PublicCards; };

		FORCEINLINE const FJoustPlayerStats& GetPlayerACurrentStats() const { return PlayerACurrentStats; };
		
		FORCEINLINE const FJoustPlayerStats& GetPlayerBCurrentStats() const { return PlayerBCurrentStats; };

		FORCEINLINE bool IsRoundPrepared() const { return bRoundPrepared; }

		FORCEINLINE bool IsStrategyFinalized() const { return bStrategyFinalized; }
};
