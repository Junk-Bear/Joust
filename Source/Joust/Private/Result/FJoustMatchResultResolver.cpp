// Fill out your copyright notice in the Description page of Project Settings.


#include "Result/FJoustMatchResultResolver.h"
#include "Result/JoustResultTypes.h"

FJoustMatchResult FJoustMatchResultResolver::Resolve(
	int32 CurrentRoundNumber, int32 BaseRoundCount, 
	int32 PlayerAFinalScore, int32 PlayerBFinalScore, 
	bool bPlayerAUnhorsed, bool bPlayerBUnhorsed)
{
	//MatchResult의 디폴트값으로 선언&정의
	FJoustMatchResult Result;

	Result.MatchOutcome = EJoustMatchOutcome::Undecided;
	Result.PlayerAScore = PlayerAFinalScore;
	Result.PlayerBScore = PlayerBFinalScore;
	Result.bMatchEndedByUnhorsed = false;
	Result.LastRoundNumber = CurrentRoundNumber;
	Result.bWentToOvertime = CurrentRoundNumber > BaseRoundCount;

	//동시 낙마 했는지 체크
	if (bPlayerAUnhorsed && bPlayerBUnhorsed)
	{
		Result.MatchOutcome = EJoustMatchOutcome::Draw;
		Result.bMatchEndedByUnhorsed = true;

		return Result;
	}

	//단일 낙마 체크(B낙마, A승리)
	if (bPlayerBUnhorsed)
	{
		Result.MatchOutcome = EJoustMatchOutcome::PlayerAWin;
		Result.bMatchEndedByUnhorsed = true;

		return Result;
	}

	//단일 낙마 체크(A낙마, B승리)
	if (bPlayerAUnhorsed)
	{
		Result.MatchOutcome = EJoustMatchOutcome::PlayerBWin;
		Result.bMatchEndedByUnhorsed = true;

		return Result;
	}

	//기본 라운드 수에 도달 못함
	if (CurrentRoundNumber < BaseRoundCount)
	{
		return Result;
	}

	//기본 라운드 이상이면 현재 누적 점수 비교, 동점은 연장전
	if (PlayerAFinalScore > PlayerBFinalScore)
	{
		Result.MatchOutcome = EJoustMatchOutcome::PlayerAWin;

		return Result;
	}
	else if (PlayerAFinalScore < PlayerBFinalScore)
	{
		Result.MatchOutcome = EJoustMatchOutcome::PlayerBWin;

		return Result;
	}
	else
	{
		return Result;
	}

}
