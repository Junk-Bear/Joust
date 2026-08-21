// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TimerManager.h"
#include "JoustPhaseCoordinator.generated.h"

class UWorld;

enum class EJoustPhase : uint8;


/**
 * 한 Phase의 시간과 Player A, B의 완료 상태를 관리하는 클래스
 */
UCLASS()
class JOUST_API UJoustPhaseCoordinator : public UObject
{
	GENERATED_BODY()
	
public: // ########## 델리게이트 블록 ##########
	/** 페이즈가 종료되었음을 알리는 이벤트 */
	DECLARE_EVENT_OneParam(UJoustPhaseCoordinator, FOnPhaseEnded, EJoustPhase);

public: // ########## public 함수 블록 ##########
	/**
	* PhaseCoordinator 초기화 
	* 
	* Timer를 사용하여 World를 명시적으로 주입함
	*/
	void Initialize(UWorld* InWorld);

	/** Strategy / Attack / Defense 처럼 제한 시간이 있는 Phase */
	bool StartTimedPhase(EJoustPhase InPhase, float InDurationSeconds, float InShortenRemainingTime);
	
	/**
	* 상위 Coordinator가 강제로 현재 Timed Phase를 종료할 경우 사용됨
	* 
	* 일반적으론 timeout 또는 양쪽 완료확인이 종료 조건
	*/
	void ForceEndTimedPhase();

	/** Player A / B가 현재 TimedPhase의 입력을 완료했음을 알림	*/
	void MarkPlayerAComplete();
	void MarkPlayerBComplete();

	/** 제한시간이 없는 페이즈를 세팅 */
	bool SetNoneTimedPhase(EJoustPhase InPhase);

	/** 남은 시간 계산 및 반환 */
	float GetRemainingTime() const;

protected: // ########## protected 함수 블록 ##########

	/** PhaseCoordinator가 사라질 때 타이머콜백이 남아있지 않도록 명시적 처리 */
	virtual void BeginDestroy() override;

private : // ########## private 함수 블록 ##########

	/** 한 플레이어가 완료했을 때 시간 단축을 처리 */
	void HandleOnePlayerComplete();

	/** A / B 완료 확인 후 현재 Phase를 조기 종료 */
	void TryShortenTime();

	/** TimedPhase의 실제 종료 처리 */
	void FinishTimedPhase();

private : // ########## private 변수 블록 ##########

	/** 월드참조, 약한참조로 검 */
	TWeakObjectPtr<UWorld> World;

	/** 타이머 핸들러 */
	FTimerHandle PhaseEndTimerHandle;

	/** 현재 페이즈가 뭔지 */
	EJoustPhase CurrentPhase{};

	/** 시작시간, 종료시간, 남은시간 */
	float PhaseStartTime = 0.0f;
	float PhaseEndTime = 0.0f;
	float PhaseDuration = 0.0f;

	/** 한플레이어 선택 완료시 단축되어버리는 시각 */
	float ShortenRemainingTime = 0.0f;

	/** 한쪽 플레이어의 완료 */
	bool bPlayerAComplete = false;
	bool bPlayerBComplete = false;

	/** 페이즈가 아직 진행중인지 */
	bool bPhaseActive = false;

	/** 실제 페이즈종료 이벤트 인스턴스 */
	FOnPhaseEnded PhaseEndedEvent;

public: // ########## GET SET 블록 ##########
	FORCEINLINE EJoustPhase GetCurrentPhase() const { return CurrentPhase; }

	FORCEINLINE bool IsPhaseActive() const { return bPhaseActive; }

	FORCEINLINE bool IsPlayerAComplete() const { return bPlayerAComplete; }

	FORCEINLINE bool IsPlayerBComplete() const { return bPlayerBComplete; }

	FORCEINLINE bool AreBothPlayerComplete() const { return bPlayerAComplete && bPlayerBComplete; }

	FORCEINLINE float GetPhaseStartTime() const { return PhaseStartTime; }

	FORCEINLINE float GetPhaseEndTime() const { return PhaseEndTime; }

	FORCEINLINE float GetPhaseDuration() const { return PhaseDuration; }

	FORCEINLINE FOnPhaseEnded& OnPhaseEnded() { return PhaseEndedEvent; }
};
