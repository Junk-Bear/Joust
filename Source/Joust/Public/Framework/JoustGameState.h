// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Common/JoustCommonTypes.h"
#include "Result/JoustResultTypes.h"
#include "JoustGameState.generated.h"

/**
 * 모든 클라이언트가 알아도 되는 공개 경기 상태를 보관
 */
UCLASS()
class JOUST_API AJoustGameState : public AGameStateBase
{
	GENERATED_BODY()
public: // ########## public 함수 블록 ##########

	/** 새 경기 시작 전 공개 상태 초기화 */
	void ResetMatchState();

	/** 현재 Phase와 서버 기준 종료 시각 설정 */
	void SetPhaseState(EJoustPhase InPhase, float InPhaseEndTime);

	/** 현재 누적 점수 설정 */
	void SetScores(int32 InPlayerAScore, int32 InPlayerBScore);

	/** 이번 Strategy Phase의 공통 공개 카드 설정 */
	void SetPublicStrategyCardIDs(const TArray<FName>& InCardIDs);

	/** 현재 라운드의 공개 카드 / 봉인 상태 초기화 */
	void ClearStrategyState();

	/** 해당 플레이어에게 봉인되어 선택 불가능한 카드 설정 */
	void SetBannedCardIDForPlayer(bool bTargetPlayerA, FName InCardID);

	/** 직전 Resolve가 완료된 RoundResult 저장 */
	void SetLastRoundResult(const FJoustRoundResult& InRoundResult);

	/** 최종 MatchResult 저장 및 경기 종료 상태 확정 */
	void SetMatchResult(const FJoustMatchResult& InMatchResult);

protected: // ########## protected 함수 블록 ##########

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private: // ########## private 변수 블록 ##########

	/** 현재 경기 라운드 번호 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Joust|Match", meta = (AllowPrivateAccess = "true"))
	int32 CurrentRoundNumber = 0;

	/** 현재 공개 Phase */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Joust|Match", meta = (AllowPrivateAccess = "true"))
	EJoustPhase CurrentPhase = EJoustPhase::Strategy;

	/**
	 * 현재 Phase의 서버 World Time 기준 종료 시각
	 * Non-Timed Phase에서는 시작/종료 시각이 동일하다
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Joust|Match", meta = (AllowPrivateAccess = "true"))
	float PhaseEndTime = 0.0f;

	/** 현재 누적 점수 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Joust|Match", meta = (AllowPrivateAccess = "true"))
	int32 PlayerAScore = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Joust|Match", meta = (AllowPrivateAccess = "true"))
	int32 PlayerBScore = 0;

	/** 이번 라운드 공통 공개 Strategy Card ID 목록 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Joust|Strategy", meta = (AllowPrivateAccess = "true"))
	TArray<FName> PublicStrategyCardIDs;

	/** Player A가 선택할 수 없도록 봉인된 카드 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Joust|Strategy", meta = (AllowPrivateAccess = "true"))
	FName BannedCardIDForPlayerA = NAME_None;

	/** Player B가 선택할 수 없도록 봉인된 카드 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Joust|Strategy", meta = (AllowPrivateAccess = "true"))
	FName BannedCardIDForPlayerB = NAME_None;

	/** Resolve 완료 후의 직전 RoundResult */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Joust|Result", meta = (AllowPrivateAccess = "true"))
	FJoustRoundResult LastRoundResult;

	/** 최종 경기 결과 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Joust|Result", meta = (AllowPrivateAccess = "true"))
	FJoustMatchResult CurrentMatchResult;

	/** 최종 MatchResult 단계에 진입했는지 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Joust|Result", meta = (AllowPrivateAccess = "true"))
	bool bMatchFinished = false;

public: // ########## GET SET 블록 ##########

	FORCEINLINE int32 GetCurrentRoundNumber() const { return CurrentRoundNumber; }

	FORCEINLINE void SetCurrentRoundNumber(int32 InRoundNumber) { CurrentRoundNumber = InRoundNumber; }

	FORCEINLINE EJoustPhase GetCurrentPhase() const { return CurrentPhase; }

	FORCEINLINE float GetPhaseEndTime() const { return PhaseEndTime; }

	FORCEINLINE int32 GetPlayerAScore() const { return PlayerAScore; }

	FORCEINLINE int32 GetPlayerBScore() const { return PlayerBScore; }

	FORCEINLINE const TArray<FName>& GetPublicStrategyCardIDs() const { return PublicStrategyCardIDs; }

	FORCEINLINE FName GetBannedCardIDForPlayerA() const { return BannedCardIDForPlayerA; }

	FORCEINLINE FName GetBannedCardIDForPlayerB() const { return BannedCardIDForPlayerB; }

	FORCEINLINE const FJoustRoundResult& GetLastRoundResult() const { return LastRoundResult; }

	FORCEINLINE const FJoustMatchResult& GetCurrentMatchResult() const { return CurrentMatchResult; }

	FORCEINLINE bool IsMatchFinished() const { return bMatchFinished; }
};
