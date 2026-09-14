// Fill out your copyright notice in the Description page of Project Settings.


#include "Result/FJoustMatchResultResolver.h"
#include "Result/JoustResultTypes.h"

FJoustMatchResult FJoustMatchResultResolver::Resolve(
	int32 InCurrentRoundNumber, int32 InBaseRoundCount, 
	int32 InPlayerAFinalScore, int32 InPlayerBFinalScore, 
	bool bInPlayerAUnhorsed, bool bInPlayerBUnhorsed)
{
	//MatchResult의 디폴트값으로 선언&정의
	FJoustMatchResult Result{};

	Result.MatchOutcome = EJoustMatchOutcome::Undecided;
	Result.PlayerAScore = InPlayerAFinalScore;
	Result.PlayerBScore = InPlayerBFinalScore;
	Result.bMatchEndedByUnhorsed = false;
	Result.LastRoundNumber = InCurrentRoundNumber;
	Result.bWentToOvertime = InCurrentRoundNumber > InBaseRoundCount;

	//동시 낙마 했는지 체크
	if (bInPlayerAUnhorsed && bInPlayerBUnhorsed)
	{
		Result.MatchOutcome = EJoustMatchOutcome::Draw;
		Result.bMatchEndedByUnhorsed = true;

		return Result;
	}

	//단일 낙마 체크(B낙마, A승리)
	if (bInPlayerBUnhorsed)
	{
		Result.MatchOutcome = EJoustMatchOutcome::PlayerAWin;
		Result.bMatchEndedByUnhorsed = true;

		return Result;
	}

	//단일 낙마 체크(A낙마, B승리)
	if (bInPlayerAUnhorsed)
	{
		Result.MatchOutcome = EJoustMatchOutcome::PlayerBWin;
		Result.bMatchEndedByUnhorsed = true;

		return Result;
	}

	//기본 라운드 수에 도달 못함
	if (InCurrentRoundNumber < InBaseRoundCount)
	{
		return Result;
	}

	//기본 라운드 이상이면 현재 누적 점수 비교, 동점은 연장전
	if (InPlayerAFinalScore > InPlayerBFinalScore)
	{
		Result.MatchOutcome = EJoustMatchOutcome::PlayerAWin;

		return Result;
	}
	else if (InPlayerAFinalScore < InPlayerBFinalScore)
	{
		Result.MatchOutcome = EJoustMatchOutcome::PlayerBWin;

		return Result;
	}
	else
	{
		return Result;
	}

}
