// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FJoustMatchResult;

/**
 * 라운드가 끝나고 경기 자체가 끝났는지 계산
 * 
 * 중요 : 
 * MatchResult를 만드는 클래스
 */
class JOUST_API FJoustMatchResultResolver final
{
public:
	/**
	* MatchResult를 만들어서 반환함.
	*/
	static FJoustMatchResult Resolve(
		int32 CurrentRoundNumber, int32 BaseRoundCount, 
		int32 PlayerAFinalScore, int32 PlayerBFinalScore,
		bool bPlayerAUnhorsed, bool bPlayerBUnhorsed
	);
};
