// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Common/JoustCommonTypes.h"
#include "Result/JoustResultTypes.h"
#include "Prediction/JoustPredictionTypes.h"
#include "JoustGameState.generated.h"

class UJoustRuleSetDataAsset;
class AJoustPlayerState;

/**
 * 모든 클라이언트가 알아도 되는 공개 경기 상태를 보관
 */
UCLASS()
class JOUST_API AJoustGameState : public AGameStateBase
{
	GENERATED_BODY()

public: // ########## 델리게이트 블록 ##########

	/** 라운드 번호 또는 누적 점수 변경을 알리는 이벤트 */
	DECLARE_EVENT(AJoustGameState, FOnMatchStateChanged);

	/** 현재 Phase 또는 Phase 종료 시각 변경을 알리는 이벤트 */
	DECLARE_EVENT(AJoustGameState, FOnPhaseStateChanged);

	/** 공개 Strategy 카드 또는 봉인 상태 변경을 알리는 이벤트 */
	DECLARE_EVENT(AJoustGameState, FOnStrategyStateChanged);

	/** 직전 RoundResult 변경을 알리는 이벤트 */
	DECLARE_EVENT(AJoustGameState, FOnRoundResultChanged);

	/** 최종 MatchResult 변경을 알리는 이벤트 */
	DECLARE_EVENT(AJoustGameState, FOnMatchResultChanged);

	/** 공개 Prediction 표시 상태 변경 이벤트 */
	DECLARE_EVENT(AJoustGameState, FOnPredictionStateChanged);

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

	/** 직전 Resolve가 완료된 RoundResult 저장 */
	void SetLastRoundResult(const FJoustRoundResult& InRoundResult);

	/** 최종 MatchResult 저장 및 경기 종료 상태 확정 */
	void SetMatchResult(const FJoustMatchResult& InMatchResult);

	/** 현재 경기 라운드 번호 설정 */
	void SetCurrentRoundNumber(int32 InRoundNumber);

	/** 클라이언트 UI에서도 사용할 경기 RuleSet 설정 */
	void SetRuleSet(UJoustRuleSetDataAsset* InRuleSet);

	/** 이번 경기의 Player A/B PlayerState 공개 참조 설정 */
	void SetParticipantStates(AJoustPlayerState* InPlayerAState, AJoustPlayerState* InPlayerBState);

	/** 이번 Strategy Phase에서 필요한 A/B 봉인 선택 상태 설정 */
	void SetPendingStrategyBans(bool bInPlayerAPendingBan, bool bInPlayerBPendingBan);

	/** 확정된 봉인 결과를 적용하고 봉인한 플레이어의 대기 상태 종료 */
	void ApplyStrategyBan(bool bInBanningPlayerA, FName InCardID);

	/** Player A/B가 방어할 때 표시할 공개 Prediction 상태 설정 */
	void SetPredictionStates(const FJoustPredictionState& InPlayerAPredictionState, const FJoustPredictionState& InPlayerBPredictionState);

	/** 현재 공개 Prediction 상태 초기화 */
	void ClearPredictionStates();

protected: // ########## protected 함수 블록 ##########

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** 라운드 번호 또는 누적 점수가 복제됐을 때 호출 */
	UFUNCTION()
	void OnRep_MatchState();

	/** 현재 Phase 또는 Phase 종료 시각이 복제됐을 때 호출 */
	UFUNCTION()
	void OnRep_PhaseState();

	/** 공개 Strategy 카드 또는 봉인 상태가 복제됐을 때 호출 */
	UFUNCTION()
	void OnRep_StrategyState();

	/** 직전 RoundResult가 복제됐을 때 호출 */
	UFUNCTION()
	void OnRep_RoundResult();

	/** 최종 MatchResult가 복제됐을 때 호출 */
	UFUNCTION()
	void OnRep_MatchResult();

	/** 공개 Prediction 표시 상태가 복제됐을 때 호출 */
	UFUNCTION()
	void OnRep_PredictionState();

private: // ########## private 변수 블록 ##########

	/** 현재 경기 라운드 번호 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_MatchState, Category = "Joust|Match", meta = (AllowPrivateAccess = "true"))
	int32 CurrentRoundNumber = 0;

	/** 현재 공개 Phase */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_PhaseState, Category = "Joust|Match", meta = (AllowPrivateAccess = "true"))
	EJoustPhase CurrentPhase = EJoustPhase::Strategy;

	/**
	 * 현재 Phase의 서버 World Time 기준 종료 시각
	 * Non-Timed Phase에서는 시작/종료 시각이 동일
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_PhaseState, Category = "Joust|Match", meta = (AllowPrivateAccess = "true"))
	float PhaseEndTime = 0.0f;

	/** 현재 누적 점수 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_MatchState, Category = "Joust|Match", meta = (AllowPrivateAccess = "true"))
	int32 PlayerAScore = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_MatchState, Category = "Joust|Match", meta = (AllowPrivateAccess = "true"))
	int32 PlayerBScore = 0;

	/** 이번 라운드 공통 공개 Strategy Card ID 목록 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_StrategyState, Category = "Joust|Strategy", meta = (AllowPrivateAccess = "true"))
	TArray<FName> PublicStrategyCardIDs;

	/** Player A가 선택할 수 없도록 봉인된 카드 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_StrategyState, Category = "Joust|Strategy", meta = (AllowPrivateAccess = "true"))
	FName BannedCardIDForPlayerA = NAME_None;

	/** Player B가 선택할 수 없도록 봉인된 카드 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_StrategyState, Category = "Joust|Strategy", meta = (AllowPrivateAccess = "true"))
	FName BannedCardIDForPlayerB = NAME_None;

	/** Resolve 완료 후의 직전 RoundResult */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_RoundResult, Category = "Joust|Result", meta = (AllowPrivateAccess = "true"))
	FJoustRoundResult LastRoundResult;

	/** 최종 경기 결과 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_MatchResult, Category = "Joust|Result", meta = (AllowPrivateAccess = "true"))
	FJoustMatchResult CurrentMatchResult;

	/** 최종 MatchResult 단계에 진입했는지 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_MatchResult, Category = "Joust|Result", meta = (AllowPrivateAccess = "true"))
	bool bMatchFinished = false;

	/** 경기에서 사용하는 RuleSet */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_StrategyState, Category = "Joust|Rules", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UJoustRuleSetDataAsset> RuleSet = nullptr;

	/** FOnMatchStateChanged 이벤트용 */
	FOnMatchStateChanged MatchStateChangedEvent;

	/** FOnPhaseStateChanged 이벤트용 */
	FOnPhaseStateChanged PhaseStateChangedEvent;

	/** FOnStrategyStateChanged 이벤트용 */
	FOnStrategyStateChanged StrategyStateChangedEvent;

	/** FOnRoundResultChanged 이벤트용 */
	FOnRoundResultChanged RoundResultChangedEvent;

	/** FOnMatchResultChanged 이벤트용 */
	FOnMatchResultChanged MatchResultChangedEvent;

	/** 이번 경기의 Player A 참조 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_MatchState, Category = "Joust|Match", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AJoustPlayerState> PlayerAState = nullptr;

	/** 이번 경기의 Player B 참조 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_MatchState, Category = "Joust|Match", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AJoustPlayerState> PlayerBState = nullptr;
	/** Player A가 아직 봉인할 카드를 선택해야 하는지 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,ReplicatedUsing = OnRep_StrategyState, Category = "Joust|Strategy", meta = (AllowPrivateAccess = "true"))
	bool bPlayerAStrategyBanPending = false;

	/** Player B가 아직 봉인할 카드를 선택해야 하는지 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_StrategyState, Category = "Joust|Strategy", meta = (AllowPrivateAccess = "true"))
	bool bPlayerBStrategyBanPending = false;

	/** Player A가 방어할 때 표시할 공개 Prediction 상태 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_PredictionState, Category = "Joust|Prediction", meta = (AllowPrivateAccess = "true"))
	FJoustPredictionState PlayerAPredictionState;

	/** Player B가 방어할 때 표시할 공개 Prediction 상태 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_PredictionState, Category = "Joust|Prediction", meta = (AllowPrivateAccess = "true"))
	FJoustPredictionState PlayerBPredictionState;

	/** FOnPredictionStateChanged 이벤트용 */
	FOnPredictionStateChanged PredictionStateChangedEvent;

public: // ########## GET SET 블록 ##########

	FORCEINLINE int32 GetCurrentRoundNumber() const { return CurrentRoundNumber; }

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

	FORCEINLINE const UJoustRuleSetDataAsset* GetRuleSet() const { return RuleSet; }

	FORCEINLINE FOnMatchStateChanged& OnMatchStateChanged() { return MatchStateChangedEvent; }

	FORCEINLINE FOnPhaseStateChanged& OnPhaseStateChanged() { return PhaseStateChangedEvent; }

	FORCEINLINE FOnStrategyStateChanged& OnStrategyStateChanged() { return StrategyStateChangedEvent; }

	FORCEINLINE FOnRoundResultChanged& OnRoundResultChanged() { return RoundResultChangedEvent; }

	FORCEINLINE FOnMatchResultChanged& OnMatchResultChanged() { return MatchResultChangedEvent; }

	FORCEINLINE AJoustPlayerState* GetPlayerAState() const { return PlayerAState.Get(); }

	FORCEINLINE AJoustPlayerState* GetPlayerBState() const { return PlayerBState.Get(); }

	FORCEINLINE bool IsPlayerAStrategyBanPending() const { return bPlayerAStrategyBanPending; }

	FORCEINLINE bool IsPlayerBStrategyBanPending() const { return bPlayerBStrategyBanPending; }

	FORCEINLINE const FJoustPredictionState& GetPlayerAPredictionState() const { return PlayerAPredictionState; }

	FORCEINLINE const FJoustPredictionState& GetPlayerBPredictionState() const { return PlayerBPredictionState; }

	FORCEINLINE FOnPredictionStateChanged& OnPredictionStateChanged() { return PredictionStateChangedEvent; }
};
