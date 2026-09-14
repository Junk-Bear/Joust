// Fill out your copyright notice in the Description page of Project Settings.


#include "Result/FJoustRoundResolver.h"
#include "Result/JoustResultTypes.h"
#include "Attack/JoustAttackTypes.h"
#include "Defense/JoustDefenseTypes.h"
#include "Rules/JoustRuleSetDataAsset.h"
#include "Interface/JoustRandomProvider.h"
#include "Defense/FJoustDefenseResolver.h"
#include "Result/FJoustScoreResolver.h"
#include "Result/FJoustCriticalResolver.h"
#include "Attack/JoustAttackTypeDataAsset.h"

namespace Joust::Private
{
	//RoundResult의 멤버변수인 ExchangeResult를 채우는 함수
	FJoustExchangeResult ResolveExchange(
		const FJoustAttackData& InAttackData,
		const FJoustDefenseData& InDefenseData,
		float InImpactTime,
		const UJoustRuleSetDataAsset& InRuleSet,
		IJoustRandomProvider& InRandomProvider
	)
	{
		FJoustExchangeResult Result{};

		//Result 멤버변수들 값 채워넣기
		Result.AttackData = InAttackData;

		Result.DefenseResult = FJoustDefenseResolver::Resolve(InAttackData, InDefenseData, InImpactTime, InRuleSet);

		Result.ScoreDelta = FJoustScoreResolver::Resolve(Result.DefenseResult);

		const UJoustAttackTypeDataAsset* AttackTypeDataPtr = InRuleSet.AttackTypeSettings.FindRef(InAttackData.AttackType).Get();

		if (!ensureMsgf(IsValid(AttackTypeDataPtr), TEXT("Missing AttackType")))
		{
			return Result;
		}

		Result.bDefenderUnhorsed = FJoustCriticalResolver::Resolve(
			InRuleSet.BaseCriticalChance, InRuleSet.MaxCriticalChance,
			AttackTypeDataPtr->CriticalChanceMultiplier,
			Result.DefenseResult.CriticalDefenseMultiplier,
			InAttackData.Finishing, InDefenseData.Stability,
			InRandomProvider, Result.FinalCriticalChance
		);

		return Result;
	};

}

FJoustRoundResult FJoustRoundResolver::Resolve(
	int32 InRoundNumber, 
	const FJoustAttackData& InPlayerAAttackData, const FJoustDefenseData& InPlayerBDefenseData, float InPlayerAToBImpactTime, 
	const FJoustAttackData& InPlayerBAttackData, const FJoustDefenseData& InPlayerADefenseData, float InPlayerBToAImpactTime, 
	const UJoustRuleSetDataAsset& InRuleSet, IJoustRandomProvider& InRandomProvider)
{
	FJoustRoundResult Result{};

	//RoudResult값 채우기
	Result.RoundNumber = InRoundNumber;

	Result.AtoBExchangeResult = Joust::Private::ResolveExchange(InPlayerAAttackData, InPlayerBDefenseData, InPlayerAToBImpactTime, InRuleSet, InRandomProvider);

	Result.BtoAExchangeResult = Joust::Private::ResolveExchange(InPlayerBAttackData, InPlayerADefenseData, InPlayerBToAImpactTime, InRuleSet, InRandomProvider);

	const bool bPlayerBUnhorsed = Result.AtoBExchangeResult.bDefenderUnhorsed;

	const bool bPlayerAUnhorsed = Result.BtoAExchangeResult.bDefenderUnhorsed;

	Result.bMatchEndedThisRound = bPlayerAUnhorsed || bPlayerBUnhorsed;

	return Result;
	
}
