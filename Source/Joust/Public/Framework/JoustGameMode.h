// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Player/JoustPlayerTypes.h"
#include "JoustGameMode.generated.h"

class UJoustRuleSetDataAsset;
class UJoustDefaultRandomProvider;
class UJoustPhaseCoordinator;
class UJoustRoundCoordinator;
class UJoustMatchCoordinator;
class AJoustPlayerController;
class AController;
class AJoustAIController;

/**
 * 게임모드
 *
 * 경기 런타임 객체의 생성과 최상위 조립을 담당
 */
UCLASS()
class JOUST_API AJoustGameMode : public AGameModeBase
{
	GENERATED_BODY()

public: // ########## 생성자 블록 ##########

	AJoustGameMode();


public: // ########## public 함수 블록 ##########

	/** 양쪽 컨트롤러가 유효한지 확인 */
	bool AreParticipantsReady() const;

	/** 현재 참가자 구성으로 새로운 게임을 시작 */
	bool StartJoustMatch();

protected: // ########## protected 함수 블록 ##########

	/** GameState 생성 후 경기 코어 초기화 */
	virtual void InitGameState() override;

	/** Player 접속 후 경기 참가자 연결, A / B 슬롯에 배정됨 */
	virtual void PostLogin(APlayerController* NewPlayer) override;

private: // ########## private 함수 블록 ##########

	/**
	 * 경기 진행에 필요한 핵심 객체 생성 및 초기화
	 */
	bool InitializeMatchCore();

	/** 
	* 참가자를 최종 확정하고 
	* PlayerState와 RoundCoordinator를 새 경기 기준으로 준비
	*/
	bool PrepareMatchParticipants();

	/** 컨트롤러에 저장된 경기 입력을 초기화 */
	bool ResetParticipantInput(AController* InController);

	/** Round의 Timed Phase 시작 시 AI 입력 처리 */
	void HandleRoundPhaseStarted(EJoustPhase InPhase);

	/** Strategy Ban이 모두 끝났을 때 AI Strategy 선택 재시도 */
	void HandleStrategyBansCompleted();

	/** 해당 슬롯의 기본 AI Strategy 입력 제출 */
	bool SubmitAIStrategy(bool bPlayerA, AJoustAIController& AIController);

	/** Ban 완료 후 AI Strategy 카드 선택 제출 */
	bool SubmitAIStrategySelection(bool bPlayerA, AJoustAIController& AIController);

	/** 해당 슬롯의 기본 AI Attack 입력 제출 */
	bool SubmitAIAttack(bool bPlayerA, AJoustAIController& AIController);

	/** 해당 슬롯의 기본 AI Defense 입력 제출 */
	bool SubmitAIDefense(bool bPlayerA, AJoustAIController& AIController);

private: // ########## private 변수 블록 ##########

	/** 이번 경기에서 사용할 전체 RuleSet */
	UPROPERTY(EditDefaultsOnly, Category = "Joust|Rules")
	TObjectPtr<UJoustRuleSetDataAsset> RuleSet = nullptr;

	/** RandomProvider 초기 Seed */
	UPROPERTY(EditDefaultsOnly, Category = "Joust|Random")
	int32 InitialRandomSeed = 0;

	/** 경기 전체 RandomProvider */
	UPROPERTY(Transient)
	TObjectPtr<UJoustDefaultRandomProvider> RandomProvider = nullptr;

	/** Phase 진행 및 시간 관리 */
	UPROPERTY(Transient)
	TObjectPtr<UJoustPhaseCoordinator> PhaseCoordinator = nullptr;

	/** 한 Round의 전체 진행 관리 */
	UPROPERTY(Transient)
	TObjectPtr<UJoustRoundCoordinator> RoundCoordinator = nullptr;

	/** 전체 Match 진행 관리 */
	UPROPERTY(Transient)
	TObjectPtr<UJoustMatchCoordinator> MatchCoordinator = nullptr;

	/** 경기 코어 초기화 완료 여부 */
	bool bMatchCoreReady = false;

	/** 논리적 참가자 */
	TWeakObjectPtr<AController> PlayerAController;
	TWeakObjectPtr<AController> PlayerBController;

	/** Player 기본 능력치 */
	UPROPERTY(EditDefaultsOnly, Category = "Joust|Player")
	FJoustPlayerStats PlayerABaseStats;
	UPROPERTY(EditDefaultsOnly, Category = "Joust|Player")
	FJoustPlayerStats PlayerBBaseStats;

public: // ########## GET SET 블록 ##########

	FORCEINLINE bool IsMatchCoreReady() const { return bMatchCoreReady; }

	FORCEINLINE UJoustRoundCoordinator* GetRoundCoordinator() const { return RoundCoordinator; }

	FORCEINLINE UJoustMatchCoordinator* GetMatchCoordinator() const { return MatchCoordinator; }

};