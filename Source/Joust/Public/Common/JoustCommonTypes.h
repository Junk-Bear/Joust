#pragma once

#include "CoreMinimal.h"
#include "JoustCommonTypes.generated.h"

/**
 * 경기 진행 Phase
 */
UENUM(BlueprintType)
enum class EJoustPhase : uint8
{
	Strategy	UMETA(DisplayName = "Strategy"),
	Attack		UMETA(DisplayName = "Attack"),
	Defense		UMETA(DisplayName = "Defense"),
	Resolve		UMETA(DisplayName = "Resolve"),
	RoundResult	UMETA(DisplayName = "Round Result"),
	MatchResult	UMETA(DisplayName = "Match Result")
};

/**
 * 찌르기 종류
 */
UENUM(BlueprintType)
enum class EJoustAttackType : uint8
{
	Normal	UMETA(DisplayName = "Normal"),
	Strong	UMETA(DisplayName = "Strong"),
	Trick	UMETA(DisplayName = "Trick"),
	Slow	UMETA(DisplayName = "Slow")
};

/**
 * 공격점과 방패 위치 사이의 방어 판정 구역
 */
UENUM(BlueprintType)
enum class EJoustGuardZone : uint8
{
	Perfect	UMETA(DisplayName = "Perfect"),
	Good	UMETA(DisplayName = "Good"),
	Bad		UMETA(DisplayName = "Bad"),
	Outside	UMETA(DisplayName = "Outside")
};

/**
 * 수비 행동
 */
UENUM(BlueprintType)
enum class EJoustDefenseAction : uint8
{
	Guard	UMETA(DisplayName = "Guard"),
	Parry	UMETA(DisplayName = "Parry")
};

/**
 * 패링 판정 결과
 */
UENUM(BlueprintType)
enum class EJoustParryOutcome : uint8
{
	NotAttempted	UMETA(DisplayName = "Not Attempted"),
	Success			UMETA(DisplayName = "Success"),
	Failure			UMETA(DisplayName = "Failure")
};

/**
 * 최종 경기 결과
 */
UENUM(BlueprintType)
enum class EJoustMatchOutcome : uint8
{
	Undecided	UMETA(DisplayName = "Undecided"),
	PlayerAWin	UMETA(DisplayName = "Player A Win"),
	PlayerBWin	UMETA(DisplayName = "Player B Win"),
	Draw		UMETA(DisplayName = "Draw")
};