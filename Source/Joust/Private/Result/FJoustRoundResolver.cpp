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
		const FJoustAttackData& AttackData,
		const FJoustDefenseData& DefenseData,
		float ImpactTime,
		const UJoustRuleSetDataAsset& RuleSet,
		IJoustRandomProvider& RandomProvider
	)
	{
		FJoustExchangeResult Result{};

		//Result 멤버변수들 값 채워넣기
		Result.AttackData = AttackData;

		Result.DefenseResult = FJoustDefenseResolver::Resolve(AttackData, DefenseData, ImpactTime, RuleSet);

		Result.ScoreDelta = FJoustScoreResolver::Resolve(Result.DefenseResult);

		const UJoustAttackTypeDataAsset* AttackTypeData = RuleSet.AttackTypeSettings.FindRef(AttackData.AttackType).Get();

		if (!ensureMsgf(AttackTypeData != nullptr, TEXT("Missing AttackType")))
		{
			return Result;
		}

		Result.bDefenderUnhorsed = FJoustCriticalResolver::Resolve(
			RuleSet.BaseCriticalChance, RuleSet.MaxCriticalChance,
			AttackTypeData->CriticalChanceMultiplier,
			Result.DefenseResult.CriticalDefenseMultiplier,
			AttackData.Finishing, DefenseData.Stability,
			RandomProvider, Result.FinalCriticalChance
		);

		return Result;
	};

}

FJoustRoundResult FJoustRoundResolver::Reslove(
	int32 RoundNumber, 
	const FJoustAttackData& PlayerAAttackData, const FJoustDefenseData& PlayerBDefenseData, float PlayerAToBImpactTime, 
	const FJoustAttackData& PlayerBAttackData, const FJoustDefenseData& PlayerADefenseData, float PlayerBToAImpactTime, 
	const UJoustRuleSetDataAsset& RuleSet, IJoustRandomProvider& RandomProvider)
{
	FJoustRoundResult Result{};

	//RoudResult값 채우기
	Result.RoundNumber = RoundNumber;

	Result.AtoBExchangeResult = Joust::Private::ResolveExchange(PlayerAAttackData, PlayerBDefenseData, PlayerAToBImpactTime, RuleSet, RandomProvider);

	Result.BtoAExchangeResult = Joust::Private::ResolveExchange(PlayerBAttackData, PlayerADefenseData, PlayerBToAImpactTime, RuleSet, RandomProvider);

	const bool bPlayerBUnhorsed = Result.AtoBExchangeResult.bDefenderUnhorsed;

	const bool bPlayerAUnhorsed = Result.BtoAExchangeResult.bDefenderUnhorsed;

	Result.bMatchEndedThisRound = bPlayerAUnhorsed || bPlayerBUnhorsed;

	return Result;
	
}
