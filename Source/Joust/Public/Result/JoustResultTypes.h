#pragma once

#include "CoreMinimal.h"
#include "Attack/JoustAttackTypes.h"
#include "Defense/JoustDefenseTypes.h"
#include "Common/JoustCommonTypes.h"
#include "JoustResultTypes.generated.h"

/**
* 한 방향에서의 공격 결과(A -> B or B -> A)
*/
USTRUCT(BlueprintType)
struct JOUST_API FJoustExchangeResult
{
	GENERATED_BODY()

	/** Phase2에서 확정된 공격 데이터 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Result")
	FJoustAttackData AttackData;

	/** 계산이 끝난 방어 데이터 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Result")
	FJoustDefenseResult DefenseResult;

	/**
	* 이번 공격으로 득점했는지
	* 1 = 득점, 0 = 무득점
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Result")
	int32 ScoreDelta = 0;

	/** 
	* 모든 값을 계산한 최종 결정타 확률
	* 
	* 결정타 Roll 성공 = 낙마
	* 
	* 최종 결정타 확률은 CriticalResolver에서 계산됨 
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Result")
	float FinalCriticalChance = 0.0f;

	/** 수비자의 낙마 여부 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Result")
	bool bDefenderUnhorsed = fasle;
};

/**
* 한 라운드의 전체 결과
* A-> B, B-> A 두 방향의 공격 결과를 모두 포함
* 따라서 양쪽 exchangeResult를 보관
*/
USTRUCT(BlueprintType)
struct JOUST_API FJoustRoundResult
{
	GENERATED_BODY()

	/** 현재 라운드 번호 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Result")
	int32 RoundNumber = 0;

	/** Player A -> Player B 공격 결과 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Result")
	FJoustExchangeResult AtoBExchangeResult;
	/** Player B -> Player A 공격 결과 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Result")
	FJoustExchangeResult BtoAExchangeResult;

	/** 이번 Round Resolve의 결과로 경기종료가 되었는지 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Result")
	bool bMatchEndedThisRound = false;
};


/** 
* 최종 경기 결과
*/
USTRUCT(BlueprintType)
struct JOUST_API FJoustMatchResult
{
	GENERATED_BODY()

	/** 최종 경기 결과 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Result")
	EJoustMatchOutcome MatchOutcome = EJoustMatchOutcome::Undecided;

	/** Player A 최종 점수 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Result")
	int32 PlayerAScore = 0;

	/** Player B 최종 점수 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Result")
	int32 PlayerBScore = 0;

	/** 낙마로 인한 경기 종료인지 체크용 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Result")
	bool bMatchEndedByUnhorsed = false;

	/** 마지막으로 진행된 라운드 번호 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Result")
	int32 LastRoundNumber = 0;

	/** 경기가 연장전으로 진행되었는지 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Result")
	bool bWentToOvertime = false;
};