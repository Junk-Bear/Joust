// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Tickable.h"
#include "Prediction/JoustPredictionTypes.h"
#include "JoustPredictionSeriesController.generated.h"

class UJoustPredictionInterpolation;

/**
 * Real 1개 + Fake 0~N개의 전체 Stage 재생 시간축을 관리하는 클래스
 */
UCLASS()
class JOUST_API UJoustPredictionSeriesController final : public UObject, public FTickableGameObject
{
	GENERATED_BODY()

public: // ########## 생성자 소멸자용 블록 ##########

	UJoustPredictionSeriesController();

public: // ########## 델리게이트 블록 ##########

	/** 한 Stage Transition이 완료됨을 알리는 이벤트 */
	DECLARE_EVENT_OneParam(UJoustPredictionSeriesController, FOnStageCompleted, int32);

	/** 전체 Real Prediction 재생이 완료됨을 알리는 이벤트 */
	DECLARE_EVENT(UJoustPredictionSeriesController, FOnPlaybackCompleted);

public: // ########## public 함수 블록 ##########

	/** 내부 Prediction 보간 객체를 생성 */
	void Initialize();

	/** Real / Fake Prediction Series 재생 시작 */
	bool StartPlayback(
		const FJoustPredictionSettings& InSettings,
		const FJoustPredictionSeries& InRealSeries,
		const TArray<FJoustPredictionSeries>& InFakeSeries);

	/** 현재 재생을 중단하고 데이터를 초기화 */
	void StopPlayback();

	/** 현재 재생 상태를 외부 공개용 Prediction State로 구성 */
	void BuildPredictionState(FJoustPredictionState& OutState) const;


protected: // ########## protected 함수 블록 ##########

	/** UObject 파괴 전 Tick을 중지 */
	virtual void BeginDestroy() override;	
	
	/** TickableGameObject */
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual UWorld* GetTickableGameObjectWorld() const override;

private: // ########## private 함수 블록 ##########

	/** 재생에 필요한 설정값과 시리즈 구성이 유효한지 확인 */
	bool ValidatePlaybackData(
		const FJoustPredictionSettings& InSettings,
		const FJoustPredictionSeries& InRealSeries,
		const TArray<FJoustPredictionSeries>& InFakeSeries) const;

	/** 최초 Stage의 표시원을 구성 */
	void InitializeDisplayCircles();

	/** 현재 Stage시간에 맞춰 Real/Fake 표시 원을 갱신 */
	bool UpdateDisplayCircles();

	/** 현재 Stage를 완료하고 다음 Stage로 진행 */
	void AdvanceStage();

	/** 마지막 Real Stage 재생을 완료한다 */
	void CompletePlayback();

	/** 재생 데이터를 초기 상태로 되돌린다. */
	void ResetPlaybackData();

private: // ########## private 변수 블록 ##########

	/** 한 Stage의 원 보간을 담당 */
	UPROPERTY(Transient)
	TObjectPtr<UJoustPredictionInterpolation> Interpolation;

	/** 현재 공격의 공통적인 설정값 */
	FJoustPredictionSettings Settings;

	/** 실제 AttackPoint를 향하는 Real Series */
	FJoustPredictionSeries RealSeries;

	/** FakeAttackPoint를 향하는 Fake Series 들 */
	TArray<FJoustPredictionSeries> FakeSeries;

	/** 현재 표시중인 Real / Fake 원 */
	FJoustPredictionDisplayCircle RealDisplayCircle;
	TArray<FJoustPredictionDisplayCircle> FakeDisplayCircles;

	/** 현재 원 -> 다음 원의 시작 원 인덱스 */
	int32 CurrentStageIdx = 0;

	/** 현재 Stage에서 경과한 시간 */
	float StageElapsedTime = 0.0f;

	/** 현재 Stage 보간 진행도 */
	float CurrentAlpha = 0.0f;

	/** 현재 재생 중인지 확인 */
	bool bIsPlaying = false;

	/** 마지막 Real Stage까지 완료했는지 확인 */
	bool bCompleted = false;

	/** FOnStageCompleted 이벤트용*/
	FOnStageCompleted StageCompletedEvent;

	/** FOnPlaybackCompleted 이벤트용 */
	FOnPlaybackCompleted PlaybackCompletedEvent;	

public: // ########## GET SET 블록 ##########

	FORCEINLINE bool IsPlaying() const { return bIsPlaying; }

	FORCEINLINE bool IsCompleted() const { return bCompleted; }

	FORCEINLINE int32 GetCurrentStageIndex() const { return CurrentStageIdx; }

	FORCEINLINE float GetCurrentAlpha() const { return CurrentAlpha; }

	FORCEINLINE FOnStageCompleted& OnStageCompleted() { return StageCompletedEvent; }

	FORCEINLINE FOnPlaybackCompleted& OnPlaybackCompleted() { return PlaybackCompletedEvent; }
};
