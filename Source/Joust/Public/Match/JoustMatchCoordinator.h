// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Result/JoustResultTypes.h"
#include "JoustMatchCoordinator.generated.h"

class UJoustRoundCoordinator;
class UJoustPhaseCoordinator;
class UJoustRuleSetDataAsset;

/**
 * 게임 매치 전체 진행을 담당하는 클래스
 */
UCLASS()
class JOUST_API UJoustMatchCoordinator : public UObject
{
	GENERATED_BODY()

	friend class UJoustRoundCoordinator;
	
public: // ########## 델리게이트 블록 ##########

	/** 최종 결과가 확정 됐음을 알리는 이벤트 */
	DECLARE_EVENT_OneParam(UJoustMatchCoordinator, FOnMatchResult, const FJoustMatchResult&);


private: // ########### 내부용 ENUM ###########

	/** 클래스 내부 진행 상태 */
	enum class EMatchFlowState : uint8
	{
		Idle,
		ReadyForRound,
		RoundInProgress,
		WaitingForRoundResultCompletion,
		MatchResult,
		Finished
	};

public: // ########## public 함수 블록 ##########

	/** MatchCoordinator 초기화 */
	void Initialize(
		UJoustRoundCoordinator* InRoundCoordinator,
		UJoustPhaseCoordinator* InPhaseCoordinator,
		const UJoustRuleSetDataAsset* InRuleSet
	);

	/** 새로운 경기를 시작 */
	bool StartMatch();

	/** 최종 MatchResult 단계 표시 / 연출 끝났음을 알림 */
	bool CompleteMatchResultPhase();

protected: // ########## protected 함수 블록 ##########

	/** WeakParent 명시적 해제 */
	virtual void BeginDestroy() override;

private: // ########## private 함수 블록 ##########

	/** 새 라운드를 시작 */
	bool StartCurrentRound();

	/** 
	* RoundResolver 끝나고 RoundCoordinator가 직접 호출함
	* 
	* 누적 점수 반영, 낙마상태 확인, MatchResultResolver호출하기 
	*/
	void HandleRoundResolved(FJoustRoundResult& RoundResult);

	/**
	* RoundResult 연출 끝나고 RoundCoordinator가 직접 호출함.
	* 
	* Pending MatchResult기준 다음라운드 or MatchResult로 진행시킴
	*/
	void HandleRoundResolvedCompleted();

	/** 경기 전체 런타임 데이터 초기화 */
	void ResetMatchData();

private: // ########### private 변수 블록 ############

	/** MatchCoordinator가 소유하고 있는 현재 RoundCoordinator */
	UPROPERTY(Transient)
	TObjectPtr<UJoustRoundCoordinator> RoundCoordinator = nullptr;

	/** RoundCoordinator가 소유하고 있는 PhaseCoordinator */
	TWeakObjectPtr<UJoustPhaseCoordinator> PhaseCoordinator = nullptr;

	/** 기본 경기 라운드 수 */
	int32 BaseRoundCount = 0;

	/** 현재 라운드 번호 */
	int32 CurrentRoundNumber = 0;

	/** Player A / B 누적 점수 */
	int32 PlayerAScore = 0;
	int32 PlayerBScore = 0;

	/** Match 계층의 내부 현재 진행 상태 */
	EMatchFlowState FlowState = EMatchFlowState::Idle;

	/** MatchResultResolver 결과 */
	FJoustMatchResult CurrentMatchResult{};

	/** 최종 경기 결과 Event */
	FOnMatchResult MatchResultEvent;

public: // ########## GET SET 블록 ##########

	FORCEINLINE int32 GetCurrentRoundNumber() const { return CurrentRoundNumber; }

	FORCEINLINE int32 GetBaseRoundCount() const { return BaseRoundCount; }

	FORCEINLINE int32 GetPlayerAScore() const { return PlayerAScore; }

	FORCEINLINE int32 GetPlayerBScore() const { return PlayerBScore; }

	FORCEINLINE bool IsOvertime() const { return CurrentRoundNumber > BaseRoundCount; }

	FORCEINLINE const FJoustMatchResult& GetCurrentMatchResult() const { return CurrentMatchResult; }

	FORCEINLINE FOnMatchResult& OnMatchResult() { return MatchResultEvent; }
};
