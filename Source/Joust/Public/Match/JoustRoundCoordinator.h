// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Attack/JoustAttackTypes.h"
#include "Defense/JoustDefenseTypes.h"
#include "Result/JoustResultTypes.h"
#include "JoustRoundCoordinator.generated.h"

class UJoustPhaseCoordinator;
class UJoustRuleSetDataAsset;
class IJoustRandomProvider;
class UJoustMatchCoordinator;
class UJoustStrategyService;
class UJoustAttackService;

struct FJoustRoundResult;



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
	bool BeginDefensePhase(float InPlayerADefenseDuration, float InPlayerBDefenseDuration);

	/** 현재 TimedPhase에서 A / B 가 입력 완료 */
	void MarkPlayerAComplete();
	void MarkPlayerBComplete();

	/**
	* Defense 종료 후 이번 라운드의 결과를 계산
	* 
	* 판정은 FJoustRoundResolver가 함
	*/
	bool ResolveRound(
		const FJoustAttackData& InAAttackData, const FJoustDefenseData& InBDefenseData, float InAToBImpactTime,
		const FJoustAttackData& InBAttackData, const FJoustDefenseData& InADefenseData, float InBToAImpactTime
	);

	/** 
	* RoundResult의 표시 / 연출이 끝났음을 알림
	* 
	* 호출 후 : EStateFlow = Finished
	*/
	bool CompleteRoundResultPhase();

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

	/** 실제 RoundResult 이벤트 인스턴스 */
	FOnRoundResult RoundResultEvent;

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
};
