// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Player/JoustPlayerTypes.h"
#include "JoustPlayerState.generated.h"

/**
 * 플레이어 1명의 경기 상태를 보관
 */
UCLASS()
class JOUST_API AJoustPlayerState : public APlayerState
{
	GENERATED_BODY()

public: // ########## public 함수 블록 ##########

	/** 기본 능력치를 설정하고 CurrentStats도 동일한 값으로 초기화 */
	void InitializeStats(const FJoustPlayerStats& InBaseStats);

	/** 새 라운드 시작 전에 라운드 단위 상태를 초기화 */
	void ResetRoundState();

	/** 새 경기 시작 전에 경기 단위 상태를 초기화 */
	void ResetMatchState();

	/** AttackType의 현재 남은 사용 횟수 Snapshot 설정 */
	void SetRemainingAttackUses(EJoustAttackType AttackType, int32 RemainingUses);

	/** AttackType의 현재 남은 사용 횟수 반환 */
	int32 GetRemainingAttackUses(EJoustAttackType AttackType) const;

	/** Resolve가 완료된 공격 기록 추가 */
	void AddAttackHistory(const FJoustAttackHistory& InAttackHistory);

	/** Resolve가 완료된 수비 기록 추가 */
	void AddDefenseHistory(const FJoustDefenseHistory& InDefenseHistory);

	/** AttackType 전체 사용 횟수 Snapshot을 교체 */
	void SetAttackUsageSnapshot(const TMap<EJoustAttackType, int32>& InRemainingAttackUses);

protected: // ########## protected 함수 블록 ##########

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private: // ########## private 변수 블록 ##########

	/** Strategy 효과 적용 전 기본 능력치 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Joust|Player", meta = (AllowPrivateAccess = "true"))
	FJoustPlayerStats BaseStats;

	/** 현재 라운드 Strategy 효과 적용 후 능력치 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Joust|Player", meta = (AllowPrivateAccess = "true"))
	FJoustPlayerStats CurrentStats;

	/** 현재 라운드에 확정된 Strategy Card ID */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Joust|Player", meta = (AllowPrivateAccess = "true"))
	FName SelectedStrategyCardID = NAME_None;

	/**
	 * 경기 전체 AttackType별 남은 사용 횟수 Snapshot
	 *
	 * INDEX_NONE = 무제한
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Joust|Player", meta = (AllowPrivateAccess = "true"))
	TMap<EJoustAttackType, int32> RemainingAttackUses;

	/** RemainingAttackUses의 네트워크 복제용 Snapshot */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Joust|Player", meta = (AllowPrivateAccess = "true"))
	TArray<FJoustAttackUsageState> ReplicatedAttackUsageStates;

	/** Resolve가 완료된 공격 기록 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Joust|Player", meta = (AllowPrivateAccess = "true"))
	TArray<FJoustAttackHistory> AttackHistory;

	/** Resolve가 완료된 수비 기록 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Joust|Player", meta = (AllowPrivateAccess = "true"))
	TArray<FJoustDefenseHistory> DefenseHistory;

	/** 직전 Resolve에서 확정된 수비 결과 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Joust|Player", meta = (AllowPrivateAccess = "true"))
	FJoustDefenseResult LastDefenseResult;

	/** 다음 Strategy Phase에서 사용할 카드 봉인권 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Joust|Player", meta = (AllowPrivateAccess = "true"))
	bool bHasBanRight = false;

	/** 현재 경기에서 낙마했는지 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Joust|Player", meta = (AllowPrivateAccess = "true"))
	bool bUnhorsed = false;

public: // ########## GET SET 블록 ##########

	FORCEINLINE const FJoustPlayerStats& GetBaseStats() const { return BaseStats; }

	FORCEINLINE const FJoustPlayerStats& GetCurrentStats() const { return CurrentStats; }

	FORCEINLINE void SetCurrentStats(const FJoustPlayerStats& InCurrentStats) { CurrentStats = InCurrentStats; }

	FORCEINLINE FName GetSelectedStrategyCardID() const { return SelectedStrategyCardID; }

	FORCEINLINE void SetSelectedStrategyCardID(FName InCardID) { SelectedStrategyCardID = InCardID; }

	FORCEINLINE const TMap<EJoustAttackType, int32>& GetRemainingAttackUses() const { return RemainingAttackUses; }

	FORCEINLINE const TArray<FJoustAttackHistory>& GetAttackHistory() const { return AttackHistory; }

	FORCEINLINE const TArray<FJoustDefenseHistory>& GetDefenseHistory() const { return DefenseHistory; }

	FORCEINLINE const FJoustDefenseResult& GetLastDefenseResult() const { return LastDefenseResult; }

	FORCEINLINE void SetLastDefenseResult(const FJoustDefenseResult& InDefenseResult) { LastDefenseResult = InDefenseResult; }

	FORCEINLINE bool HasBanRight() const { return bHasBanRight; }

	FORCEINLINE void SetHasBanRight(bool bInHasBanRight) { bHasBanRight = bInHasBanRight; }

	FORCEINLINE bool IsUnhorsed() const { return bUnhorsed; }

	FORCEINLINE void SetUnhorsed(bool bInUnhorsed) { bUnhorsed = bInUnhorsed; }

	FORCEINLINE const TArray<FJoustAttackUsageState>& GetReplicatedAttackUsageStates() const { return ReplicatedAttackUsageStates; }
};
