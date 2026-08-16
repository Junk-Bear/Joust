// Fill out your copyright notice in the Description page of Project Settings.


#include "Match/JoustPhaseCoordinator.h"
#include "Common/JoustCommonTypes.h"
#include "Engine/World.h"

bool UJoustPhaseCoordinator::SetNoneTimedPhase(EJoustPhase InPhase)
{
	if (bPhaseActive)
		return false;

	CurrentPhase = InPhase;

	PhaseDuration = 0.0f;
	ShortenRemainingTime = 0.0f;

	bPlayerAComplete = false;
	bPlayerBComplete = false;

	if (UWorld* WorldPtr = World.Get())
	{
		const float CurrentTime = WorldPtr->GetTimeSeconds();
		
		PhaseStartTime = CurrentTime;
		PhaseEndTime = CurrentTime;
	}
	else
	{
		PhaseStartTime = 0.0f;
		PhaseEndTime = 0.0f;
	}

	return true;
}

float UJoustPhaseCoordinator::GetRemainingTime() const
{
	if (!bPhaseActive)
		return 0.0f;

	const UWorld* WorldPtr = World.Get();

	if (WorldPtr == nullptr)
		return 0.0f;

	return FMath::Max(0.0f, PhaseEndTime - WorldPtr->GetTimeSeconds());
}

bool UJoustPhaseCoordinator::StartTimedPhase(EJoustPhase InPhase, float InDurationSeconds, float InShortenRemainingTime)
{
	UWorld* WorldPtr = World.Get();

	//월드가 이미 없어진 상태면 타이머 사용 불가
	if (WorldPtr == nullptr)
		return false;

	//기존 TimePhase가 실행 중이면 새로 덮지 않게 
	if (bPhaseActive)
		return false;

	//0초 이하가 된 TimePhase는 허용 X
	if (InDurationSeconds <= 0.0f)
		return false;

	CurrentPhase = InPhase;

	PhaseDuration = InDurationSeconds;

	// 음수 = 의미 없음
	// 기본 Phase보다 큰 값 들어와도 필요없음
	ShortenRemainingTime = FMath::Clamp(InShortenRemainingTime, 0.0f, PhaseDuration);

	PhaseStartTime = WorldPtr->GetTimeSeconds();

	PhaseEndTime = PhaseStartTime + PhaseDuration;

	bPlayerAComplete = false;
	bPlayerBComplete = false;

	bPhaseActive = true;

	WorldPtr->GetTimerManager().SetTimer(PhaseEndTimerHandle, this, &UJoustPhaseCoordinator::FinishTimedPhase, PhaseDuration, false);

	return true;
}

bool UJoustPhaseCoordinator::NoneTimedPhase(EJoustPhase InPhase)
{
	//TimePhase가 아직 진행중이면 페이즈를 넘어가지 않게함
	if (bPhaseActive)
		return false;

	CurrentPhase = InPhase;

	UWorld* WorldPtr = World.Get();

	if (WorldPtr != nullptr)
	{
		PhaseStartTime = WorldPtr->GetTimeSeconds();

		PhaseEndTime = PhaseStartTime;
	}
	else
	{
		PhaseStartTime = 0.0f;
		PhaseEndTime = 0.0f;
	}

	PhaseDuration = 0.0f;

	ShortenRemainingTime = 0.0f;

	bPlayerAComplete = false;
	bPlayerBComplete = false;

	return true;
}

void UJoustPhaseCoordinator::ForceEndTimedPhase()
{
	//논타임페이즈나 이미 종료된 페이즈면 무시
	if (!bPhaseActive)
		return;

	FinishTimedPhase();
}

void UJoustPhaseCoordinator::MarkPlayerAComplete()
{
	//논타임페이즈나 이미 종료된 페이즈면 무시
	if (!bPhaseActive)
		return;

	//동일 플레이어 중복 완료입력 무시
	if (bPlayerAComplete)
		return;

	bPlayerAComplete = true;

	HandleOnePlayerComplete();
}

void UJoustPhaseCoordinator::MarkPlayerBComplete()
{
	//논타임페이즈나 이미 종료된 페이즈면 무시
	if (!bPhaseActive)
		return;

	//동일 플레이어 중복 완료입력 무시
	if (bPlayerBComplete)
		return;

	bPlayerBComplete = true;

	HandleOnePlayerComplete();
}

void UJoustPhaseCoordinator::BeginDestroy()
{
	UWorld* WorldPtr = World.Get();

	if (WorldPtr != nullptr)
	{
		WorldPtr->GetTimerManager().ClearTimer(PhaseEndTimerHandle);
	}

	PhaseEndTimerHandle.Invalidate();

	Super::BeginDestroy();
}

void UJoustPhaseCoordinator::HandleOnePlayerComplete()
{
	//논타임페이즈나 이미 종료된 페이즈면 무시
	if (!bPhaseActive)
		return;

	//양쪽 모두 완료 - 즉시 종료
	if (AreBothPlayerComplete())
	{
		FinishTimedPhase();
		return;
	}

	// 한쪽 완료 상태
	// 페이즈1, 2는 7초단축
	// 페이즈3은 아무 짓도 안함
	TryShortenTime();
}

void UJoustPhaseCoordinator::TryShortenTime()
{
	//논타임페이즈나 이미 종료된 페이즈면 무시
	if (!bPhaseActive)
		return;

	// 0이하 : 시간 단축 사용 안함
	// 언제 0 이하? Phase 3(Defense)
	if (ShortenRemainingTime <= 0.0f)
		return;

	UWorld* WorldPtr = World.Get();

	if (WorldPtr == nullptr)
		return;

	const float RemainingTime = GetRemainingTime();

	//이미 단축된 시간보다 짧다면 남은 시간을 변경하지 않는다 (늘어나는 경우임)
	if (RemainingTime <= ShortenRemainingTime)
		return;


	PhaseEndTime = WorldPtr->GetTimeSeconds() + ShortenRemainingTime;

	//단축된 시간으로 재설정
	WorldPtr->GetTimerManager().SetTimer(PhaseEndTimerHandle, this, &UJoustPhaseCoordinator::FinishTimedPhase, ShortenRemainingTime, false);
}

void UJoustPhaseCoordinator::FinishTimedPhase()
{
	//논타임페이즈나 이미 종료된 페이즈면 무시
	if (!bPhaseActive)
		return;

	UWorld* WorldPtr = World.Get();

	if (WorldPtr != nullptr)
	{
		// 조기 종료 였을 경우 남아있는 Timer제거
		WorldPtr->GetTimerManager().ClearTimer(PhaseEndTimerHandle);

		// 실제 페이즈 종료 시각으로 갱신
		// 일찍 완료하면 예정 종료가 아닌 실제 종료시각이 남게
		PhaseEndTime = WorldPtr->GetTimeSeconds();
	}

	const EJoustPhase EndedPhase = CurrentPhase;

	// 이벤트 방송전에 비활성화
	// 리스너가 이벤트 처리 과정에서 다시 종료를 보내도 해당 함수가 재실행 되지 않게함
	bPhaseActive = false;
	PhaseEndTimerHandle.Invalidate();

	PhaseEndedEvent.Broadcast(EndedPhase);
}
