// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Attack/JoustAttackTypes.h"
#include "Defense/JoustDefenseTypes.h"
#include "Result/JoustResultTypes.h"
#include "Prediction/JoustPredictionTypes.h"
#include "JoustRoundCoordinator.generated.h"

class UJoustPhaseCoordinator;
class UJoustRuleSetDataAsset;
class IJoustRandomProvider;
class UJoustMatchCoordinator;
class UJoustStrategyService;
class UJoustAttackService;
class UJoustPredictionService;
class UJoustPredictionSeriesController;
class IJoustStrategyInput;
class IJoustAttackInput;
class IJoustDefenseInput;
class AJoustPlayerState;
class AJoustGameState;

struct FJoustRoundResult;
struct FJoustAttackData;


/**
 * 한 라운드의 진행을 담당하는 Coordinator
 */
UCLASS()
class JOUST_API UJoustRoundCoordinator : public UObject
{
	GENERATED_BODY()
	
public: // ########## 델리게이트 블록 ##########

	/** 라운드 결과가 확정되었음을 알리는 이벤트 */
	DECLARE_EVENT_OneParam(UJoustRoundCoordinator, FOnRoundResult, const FJoustRoundResult&);

	/** 양쪽 방향 모두 Prediction 재생이 완료 되었음을 알리는 이벤트 */
	DECLARE_EVENT(UJoustRoundCoordinator, FOnPredictionPlaybackCompleted);

	/** Timed Phase가 실제 시작되었음을 알리는 이벤트 */
	DECLARE_EVENT_OneParam(UJoustRoundCoordinator, FOnPhaseStarted, EJoustPhase);

	/** Strategy의 필요한 카드 봉인이 모두 완료되었음을 알리는 이벤트 */
	DECLARE_EVENT(UJoustRoundCoordinator, FOnStrategyBansCompleted);

private: // ########## 내부용 ENUM ##########

	enum class ERoundFlowState : uint8
	{
		Idle,

		ReadyForStrategy,
		Strategy,

		ReadyForAttack,
		Attack,

		ReadyForDefense,
		Defense,

		ReadyForResolve,
		Resolving,

		RoundResult,
		
		Finished
	};

public: // ########## public 함수 블록 ##########

	/** RoundCoordinator를 초기화 */
	void Initialize(UJoustPhaseCoordinator* InPhaseCoordinator, UJoustRuleSetDataAsset* InRuleSet, IJoustRandomProvider& InRandomProvider);

	/**
	*새로운 라운드를 준비
	* 
	* 성공 시 : FlowStat = ReadyForStrategy
	*/
	bool StartRound(int32 InRoundNumber);

	/** Phase 1 - Strategy 시작 */
	bool BeginStrategyPhase();

	/** Phase 2 - Attack 시작 */
	bool BeginAttackPhase();

	/**
	*Phase 3 - Defense 시작
	* 
	* 두 방어의 제한 시간 중 더 긴 값을 제한 시간으로 사용
	*/
	bool BeginDefensePhase();

	/** 현재 TimedPhase에서 A / B 가 입력 완료 */
	void MarkPlayerAComplete();
	void MarkPlayerBComplete();

	/**
	* Defense 종료 후 이번 라운드의 결과를 계산
	* 
	* 판정은 FJoustRoundResolver가 함
	*/
	bool ResolveRound();

	/** 
	* RoundResult의 표시 / 연출이 끝났음을 알림
	* 
	* 호출 후 : EStateFlow = Finished
	*/
	bool CompleteRoundResultPhase();

	/** 양쪽 Prediction이 모두 준비되었는지 확인 */
	bool ArePredictionsPrepared() const;

	/** Strategy Phase에서 해당 플레이어의 카드 봉인 입력을 제출 */
	bool SubmitStrategyBan(bool bPlayerA, const IJoustStrategyInput& StrategyInput);

	/** Strategy Phase에서 해당 플레이어의 전략 카드 선택을 제출 */
	bool SubmitStrategySelection(bool bPlayerA, const IJoustStrategyInput& StrategyInput);

	/** Attack Phase에서 해당 플레이어의 확정 공격 입력을 제출 */
	bool SubmitAttack(bool bPlayerA, const IJoustAttackInput& AttackInput);

	/** Defense Phase에서 해당 플레이어의 방어 입력을 제출 */
	bool SubmitDefense(bool bPlayerA, const IJoustDefenseInput& DefenseInput);

	/** 실제 Player A / B PlayerState를 연결하고 현재 Attack Usage를 동기화 */
	bool SetPlayerStates(AJoustPlayerState* InPlayerAState, AJoustPlayerState* InPlayerBState);

	/** 새 경기 시작 전에 Round 계층의 경기 단위 상태를 초기화 */
	bool ResetMatchState();

	/** 해당 방어자가 현재 볼 수 있는 공개 Prediction 상태 반환 */
	bool GetDefensePredictionState(bool bPlayerA, FJoustPredictionState& OutState) const;

protected: // ########## protected 함수 블록 ##########

	/** PhaseCoordinator의 이벤트 바인딩을 명시적으로 정리 */
	virtual void BeginDestroy() override;

private: // ########## private 함수 블록 ##########

	/** 페이즈의 공통된 Time Phase 시작 처리 */
	bool BeginTimedPhase(
		EJoustPhase InPhase,
		float InDurationSeconds, float InShortenRemainingTime,
		ERoundFlowState InRequiredState, ERoundFlowState InActiveState
	);

	/** PhaseCoordinator의 이벤트 수신용(페이즈종료) */
	void HandlePhaseEnded(EJoustPhase EndedPhase);

	/** 새 라운드를 하기전 이전 라운드 초기화 */
	void ResetRoundData();

	/** 양 방향 재생 완료를 확인 */
	void HandlePredictionPlaybackCompleted();

	/** PlayerState의 Attack Usage Snapshot을 동기화 */
	bool SyncAttackUsageStates();

	/** 양쪽 확정 공격 / 방어 데이터에 대한 Prediction을 준비 */
	bool PreparePredictions();	
	
	/** 준비된 양방향 Prediction Series 재생을 시작 */
	bool StartPredictionPlayback();

	/** 확정된 RoundResult를 양쪽 PlayerState에 반영 */
	void ApplyRoundResultToPlayerStates(AJoustPlayerState& PlayerAStateRef, AJoustPlayerState& PlayerBStateRef, int32 PlayerAScore, int32 PlayerBScore);
	
	/** 현재 Phase 상태를 GameState에 동기화 */
	void SyncPhasePublicState();

	/** 이번 라운드 공개 Strategy 카드 목록을 GameState에 동기화 */
	void SyncStrategyPublicCards();

private: // ########## private 변수 블록 ##########

	/** RoundCoordinator 존재 -> PhaseCoordinator 반드시 존재 */
	UPROPERTY(Transient)
	TObjectPtr<UJoustPhaseCoordinator> PhaseCoordinator = nullptr;

	/** 경기 전체 RuleSet */
	UPROPERTY(Transient)
	TObjectPtr<UJoustRuleSetDataAsset> RuleSet = nullptr;

	/** 랜덤 생성용 */
	IJoustRandomProvider* RandomProvider = nullptr;

	/** 상위 MatchCoordinator */
	TWeakObjectPtr<UJoustMatchCoordinator> MatchCoordinator;

	/** 실제 경기 Player A / B 상태 */
	TWeakObjectPtr<AJoustPlayerState> PlayerAState;
	TWeakObjectPtr<AJoustPlayerState> PlayerBState;

	/** Phase1 시스템 */
	UPROPERTY(Transient)
	TObjectPtr<UJoustStrategyService> StrategyService = nullptr;

	/** Phase2 시스템 */
	UPROPERTY(Transient)
	TObjectPtr<UJoustAttackService> AttackService = nullptr;

	/** 현재 라운드 번호 */
	int32 RoundNumber = 0;

	/** 현재 라운드가 진행중인지 */
	bool bRoundActive = false;

	/** 현재 클래스에서 내부 진행 상태 */
	ERoundFlowState FlowState = ERoundFlowState::Idle;

	/** A / B의 확정 AttackData */
	FJoustAttackData PlayerAAttackData{};
	FJoustAttackData PlayerBAttackData{};

	/** A / B의 확정 DefenseData */
	FJoustDefenseData PlayerADefenseData{};
	FJoustDefenseData PlayerBDefenseData{};

	/** Player간의 공격 Impact 시각 */
	float AToBImpactTime = 0.0f;
	float BToAImpactTime = 0.0f;

	/** 이번 라운드의 최종 판정 결과 */
	FJoustRoundResult CurrentRoundResult{};

	/** FOnRoundResult 이벤트용 */
	FOnRoundResult RoundResultEvent;

	/** 양플레이어 공->수에 대한 Prediction */
	UPROPERTY(Transient)
	TObjectPtr<UJoustPredictionService> AToBPredictionService;
	UPROPERTY(Transient)
	TObjectPtr<UJoustPredictionService> BToAPredictionService;

	/** 방향별 Prediction 재생 */
	UPROPERTY(Transient)
	TObjectPtr<UJoustPredictionSeriesController> AToBPredictionController;
	UPROPERTY(Transient)
	TObjectPtr<UJoustPredictionSeriesController> BToAPredictionController;

	/** 양 방향 Prediction 재생이 완료됐는지 확인 */
	bool bPredictionPlaybackCompleted = false;

	/** FOnPredictionPlaybackCompleted 이벤트용 */
	FOnPredictionPlaybackCompleted PredictionPlaybackCompletedEvent;

	/** 공개 경기 상태 */
	TWeakObjectPtr<AJoustGameState> GameState;

	/** Phase 시작 이벤트용 */
	FOnPhaseStarted PhaseStartedEvent;

	/** Strategy Ban 완료 이벤트용 */
	FOnStrategyBansCompleted StrategyBansCompletedEvent;


public: // ########## GET SET 블록 ##########

	FORCEINLINE int32 GetRoundNumber() const { return RoundNumber; }

	FORCEINLINE bool IsRoundActive() const { return bRoundActive; }

	FORCEINLINE const FJoustAttackData& GetPlayerAAttackData() const { return PlayerAAttackData; }

	FORCEINLINE const FJoustAttackData& GetPlayerBAttackData() const { return PlayerBAttackData; }

	FORCEINLINE const FJoustDefenseData& GetPlayerADefenseData() const { return PlayerADefenseData; }

	FORCEINLINE const FJoustDefenseData& GetPlayerBDefenseData() const { return PlayerBDefenseData; }

	FORCEINLINE float GetAToBImpactTime() const { return AToBImpactTime; }

	FORCEINLINE float GetBToAImpactTime() const { return BToAImpactTime; }

	FORCEINLINE const FJoustRoundResult& GetCurrentRoundResult() const { return CurrentRoundResult; }

	FORCEINLINE void SetMatchCoordinator(UJoustMatchCoordinator* InMatchCoordinator) { MatchCoordinator = InMatchCoordinator; }

	FORCEINLINE FOnRoundResult& OnRoundResult() { return RoundResultEvent; }

	FORCEINLINE FOnPredictionPlaybackCompleted& OnPredictionPlaybackCompleted() { return PredictionPlaybackCompletedEvent; }

	FORCEINLINE bool IsPredictionPlaybackCompleted() const { return bPredictionPlaybackCompleted; }

	FORCEINLINE void SetGameState(AJoustGameState* InGameState) { GameState = InGameState; }

	FORCEINLINE FOnPhaseStarted& OnPhaseStarted() { return PhaseStartedEvent; }

	FORCEINLINE FOnStrategyBansCompleted& OnStrategyBansCompleted() { return StrategyBansCompletedEvent; }
};
