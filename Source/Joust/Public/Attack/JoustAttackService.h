// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Player/JoustPlayerTypes.h"
#include "Attack/JoustAttackTypes.h"
#include "Attack/FJoustAttackUsageTracker.h"
#include "JoustAttackService.generated.h"

class UJoustRuleSetDataAsset;
class IJoustRandomProvider;

/**
 * Attack Phase의 진행을 맡은 클래스
 */
UCLASS()
class JOUST_API UJoustAttackService : public UObject
{
	GENERATED_BODY()
	
public: // ########## public 함수 블록 ##########

	/** RuleSet과 RandomProvider를 설정, 초기화 */
	bool Initialize(UJoustRuleSetDataAsset* InRuleSet, IJoustRandomProvider& InRandomProvider);

	/** 새 경기 기준으로 양쪽의 사용횟수 초기화 */
	bool ResetMatchUsage();

	/** 새 라운드의 Attack 상태를 준비 */
	bool PrepareRound(const FJoustPlayerStats& InPlayerACurrentStats, const FJoustPlayerStats& InPlayerBCurrentStats);

	/** 이번 라운드 Attack상태만 초기화 */
	void EndRound();

	/** 플레이어의 공격을 검증하고 최종 AttackData로 확정 */
	bool SubmitAttack(bool bPlayerA, const FJoustAttackData& InAttackData);

	/** 해당 플레이어가 이번 라운드 공격 제출 완료했는지 확인 */
	bool IsPlayerComplete(bool bPlayerA) const;

	/** 양쪽 모두 이번 라운드 공격 제출을 완료했는지 확인 */
	bool AreBothPlayersComplete() const;

	/** 해당 플레이어가 특정 공격타입을 아직 사용할 수 있는지 확인 */
	bool CanPlayerUseAttackType(bool bPlayerA, EJoustAttackType AttackType) const;

	/** 공격 타입의 남은 횟수 반환(무제한 : INDEX_NONE) */
	int32 GetRemainingUses(bool bPlayerA, EJoustAttackType AttackType) const;

	/** 해당 플레이어의 전체 AttackType 남은 사용 횟수 Snapshot 반환 */
	bool GetUsageSnapshot(bool bPlayerA, TMap<EJoustAttackType, int32>& OutRemainingUses) const;

private: // ########## private 함수 블록 ##########
	/** RuleSet과 RandomProvider, Usage 빼고 남은 상태만 초기화 */
	void ResetRoundState();

private: // ########## private 변수 블록 ##########

	/** 룰셋 */
	UPROPERTY(Transient)
	TObjectPtr<UJoustRuleSetDataAsset> RuleSet = nullptr;

	/** 양 플레이어의 현재 능력치 */
	UPROPERTY(Transient)
	FJoustPlayerStats PlayerACurrentStats;

	UPROPERTY(Transient)
	FJoustPlayerStats PlayerBCurrentStats;

	/** 이번 라운드 확정된 서버 비공개 공격데이터 */
	UPROPERTY(Transient)
	FJoustAttackData PlayerAAttackData;

	UPROPERTY(Transient)
	FJoustAttackData PlayerBAttackData;

	/** 랜덤프로바이더 */
	IJoustRandomProvider* RandomProvider = nullptr;

	/** 플레이어의 경기 전체 AttackType사용 횟수 */
	FJoustAttackUsageTracker PlayerAUsageTracker;
	FJoustAttackUsageTracker PlayerBUsageTracker;

	/** 경기 전체 UsageTracker가 정상 준비 되었는지 확인 */
	bool bMatchUsageInitialized = false;

	/** 이번 라운드 Attack 준비 확인 */
	bool bRoundPrepared = false;

	/** 현재 공격 제출 가능한지 확인 */
	bool bSubmissionOpen = false;

	/** 이번 라운드 공격 제출 완료 확인 */
	bool bPlayerAAttackSubmitted = false;
	bool bPlayerBAttackSubmitted = false;

	
public: // ########## GET SET 블록 ###########

	FORCEINLINE void SetSubmissionOpen(bool bInSubmissionOpen) { bSubmissionOpen = bInSubmissionOpen; }

	FORCEINLINE const FJoustAttackData& GetPlayerAAttackData() const { return PlayerAAttackData; }

	FORCEINLINE const FJoustAttackData& GetPlayerBAttackData() const { return PlayerBAttackData; }

	FORCEINLINE bool IsRoundPrepared() const { return bRoundPrepared; }

	FORCEINLINE bool IsSubmissionOpen() const { return bSubmissionOpen;}

	FORCEINLINE bool IsMatchUsageInitialized() const { return bMatchUsageInitialized; }

};
