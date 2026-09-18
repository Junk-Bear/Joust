// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TimerManager.h"
#include "Common/JoustCommonTypes.h"
#include "JoustPresentationController.generated.h"

class AJoustGameState;
class AJoustHUD;
class AJoustPlayerController;
class AJoustPlayerState;
class UJoustHUDWidget;
class UJoustDefenseWidget;

/**
 * Local Player의 Presentation 흐름을 중계
 */
UCLASS()
class JOUST_API UJoustPresentationController : public UObject
{
	GENERATED_BODY()
	
public: // ########## public 함수 블록 ############

	/** Local Presentation에 필요한 런타임 참조들 초기화 */
	bool Initialize(AJoustPlayerController* InPlayerController, AJoustHUD* InHUD, UJoustHUDWidget* InRootWidget);

	/** 현재 Local Player의 PlayerState 반환 */
	AJoustPlayerState* GetPlayerState() const;

protected: // ########## protected 함수 블록 ##########

	/** GameState 이벤트 바인딩 및 약한 참조 정리 */
	virtual void BeginDestroy() override;

private: // ########## private 함수 블록 ##########

	/** GameState 이벤트 구독 */
	void BindGameStateEvents();

	/** GameState 이벤트 구독 해제 */
	void UnbindGameStateEvents();

	/** 라운드 번호 또는 점수 변경 처리 */
	void HandleMatchStateChanged();

	/** 현재 Phase 변경 처리 */
	void HandlePhaseStateChanged();

	/** 직전 RoundResult 변경 처리 */
	void HandleRoundResultChanged();

	/** 최종 MatchResult 변경 처리 */
	void HandleMatchResultChanged();

	/** 공통 HUD의 라운드·점수·Phase 텍스트 갱신 */
	void RefreshCommonHUD();

	/** 현재 공개 상태에 맞는 Phase 화면 표시 */
	void RefreshCurrentScreen();

	/** 공통 HUD 타이머 갱신 시작 */
	void StartTimerUpdates();

	/** 공통 HUD 타이머 갱신 중지 */
	void StopTimerUpdates();

	/** 서버 기준 Phase 남은 시간 갱신 */
	void UpdateRemainingTime();

	/** GameState의 공개 Strategy 상태가 변경됐을 때 호출 */
	void HandleStrategyStateChanged();

	/** 공개 카드 ID와 RuleSet을 Strategy 화면에 반영 */
	void RefreshStrategyScreen();

	/** Strategy 화면에서 선택한 카드 확정 요청을 전달 */
	void HandleStrategyConfirmed(FName InCardID);

	/** Strategy 화면에서 선택한 봉인 카드 요청을 전달 */
	void HandleStrategyBanConfirmed(FName InCardID);

	/** 공격 사용 횟수와 LanceBox 범위를 Attack 화면에 반영 */
	void RefreshAttackScreen();

	/** Attack 화면에서 확정한 공격 요청 전달 */
	void HandleAttackConfirmed(EJoustAttackType InAttackType, FVector2D InAttackPoint);

	/** 서버의 공격 요청 처리 결과를 Attack 화면에 전달 */
	void HandleAttackRequestCompleted(bool bInAccepted);

	/** START MATCH 버튼 요청 처리 */
	void HandleStartMatchRequested();

	/** 새로운 Round 또는 Phase 진입 시 화면 입력 상태 초기화 */
	void RefreshPhaseEntry();

	/** 화면별 Widget 이벤트 구독 */
	void BindWidgetEvents();

	/** 화면별 Widget 이벤트 구독 해제 */
	void UnbindWidgetEvents();

	/** LanceBox 범위를 Defense 화면에 반영 */
	void RefreshDefenseScreen();

	/** Defense 화면에서 변경된 방패 위치 전달 */
	void HandleShieldPointChanged(FVector2D InShieldPoint);

	/** Defense 화면에서 발생한 패링 요청 전달 */
	void HandleParryRequested(FVector2D InShieldPoint);

	/** 공개 Prediction 상태 변경 처리 */
	void HandlePredictionStateChanged();

	/** Local Player가 방어할 Prediction 상태를 Defense 화면에 반영 */
	void RefreshDefensePrediction();

private: // ########## private 변수 블록 ##########

	/** 이 Presentation을 소유한 Local PlayerController */
	TWeakObjectPtr<AJoustPlayerController> PlayerController;

	/** 모든 클라이언트가 읽는 공개 경기 상태 */
	TWeakObjectPtr<AJoustGameState> GameState;

	/** 실제 화면 표시를 담당하는 HUD */
	TWeakObjectPtr<AJoustHUD> HUD;

	/** 화면에 생성된 최상위 Joust HUD Widget */
	TWeakObjectPtr<UJoustHUDWidget> RootWidget;

	/** 공통 HUD 타이머 갱신 핸들 */
	FTimerHandle TimerUpdateHandle;

	/** 마지막으로 표시한 정수 단위 남은 시간 */
	int32 LastDisplayedRemainingSeconds = INDEX_NONE;

	/** 마지막으로 입력 상태를 초기화한 라운드 번호 */
	int32 LastInitializedRoundNumber = INDEX_NONE;

	/** 마지막으로 입력 상태를 초기화한 Phase */
	EJoustPhase LastInitializedPhase = EJoustPhase::Strategy;

	/** 현재 Round와 Phase의 입력 상태를 초기화했는지 */
	bool bHasInitializedPhase = false;

public: // ########## GET SET 블록 ##########

	FORCEINLINE AJoustPlayerController* GetPlayerController() const { return PlayerController.Get(); }

	FORCEINLINE AJoustGameState* GetGameState() const { return GameState.Get(); }

	FORCEINLINE AJoustHUD* GetHUD() const { return HUD.Get(); }

	FORCEINLINE UJoustHUDWidget* GetRootWidget() const { return RootWidget.Get(); }
};
