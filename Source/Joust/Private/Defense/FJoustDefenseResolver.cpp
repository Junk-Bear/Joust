// Fill out your copyright notice in the Description page of Project Settings.


#include "Defense/FJoustDefenseResolver.h"
#include "Common/JoustCommonTypes.h"
#include "Rules/JoustRuleSetDataAsset.h"
#include "Defense/JoustDefenseTypes.h"
#include "Attack/JoustAttackTypes.h"
#include "Defense/FJoustGuardResolver.h"
#include "Defense/FJoustParryResolver.h"
#include "Defense/FJoustEdgeParryResolver.h"

namespace Joust::Private
{
	//가드존에 따른 보정값들 구하는 함수
	float ResoveCriticalDefenseMultiplier(EJoustGuardZone InGuardZone, EJoustParryOutcome InParryOutcome, const UJoustRuleSetDataAsset& InRuleSet)
	{
		switch (InParryOutcome)
		{
			//패링 성공
		case EJoustParryOutcome::Success:
			return 0.0;

			//패링 실패시
		case EJoustParryOutcome::Failure:
			switch (InGuardZone)
			{
			case EJoustGuardZone::Perfect:
				return InRuleSet.ParryFailurePerfectCriticalMultiplier;

			case EJoustGuardZone::Good:
				return InRuleSet.ParryFailureGoodCriticalMultiplier;

			case EJoustGuardZone::Bad:
				return InRuleSet.ParryFailureBadCriticalMultiplier;

			case EJoustGuardZone::Outside:
			default:
				//아웃사이드 케이스 : 잘못된 경우의 수, 이미 ParryOutcom::NotAttempted여야 한다.
				ensureMsgf(false, TEXT("Outside cannot ParryOutcome-Failure"));

				return InRuleSet.GuardOutsideCriticalMultiplier;
			}

			//패링 안함 => 가드판정에 따른 배율
		case EJoustParryOutcome::NotAttempted:
			switch (InGuardZone)
			{
			case EJoustGuardZone::Perfect:
				return InRuleSet.GuardPerfectCriticalMultiplier;

			case EJoustGuardZone::Good:
				return InRuleSet.GuardGoodCriticalMultiplier;

			case EJoustGuardZone::Bad:
				return InRuleSet.GuardBadCriticalMultiplier;

			case EJoustGuardZone::Outside:
			default:
				return InRuleSet.GuardOutsideCriticalMultiplier;
			}

			//패링아웃컴이 없음 => 잘못된 경우의 수, 일단 체크 및 결정타 배율0.0
		default:
			ensureMsgf(false, TEXT("EJoustParryOutcome Invalid"));
			return 0.0f;
		}

	}
}

FJoustDefenseResult FJoustDefenseResolver::Resolve(
	const FJoustAttackData& InAttackData, 
	const FJoustDefenseData& InDefenseData, 
	float InImpactTime, 
	const UJoustRuleSetDataAsset& InRuleSet)
{
	FJoustDefenseResult Result{};

	//Result 멤버변수 값 채우기
	Result.GuardZone = FJoustGuardResolver::Resolve(
		InAttackData.AttackPoint, InDefenseData.ShieldPoint,
		InRuleSet.PerfectZoneRadius, InRuleSet.GoodZoneRadius, InRuleSet.BadZoneRadius,
		Result.HitDistance);

	Result.ParryOutcome = FJoustParryResolver::Resolve(
		InDefenseData.bParryAttempted, Result.GuardZone,
		InDefenseData.ParryInputTime, InImpactTime,
		InDefenseData.ParrySense,
		InRuleSet.PerfectZoneParryWindow, InRuleSet.GoodZoneParryWindow, InRuleSet.BadZoneParryWindow,
		Result.ParryTimingError, Result.AllowedParryWindow
	);

	Result.DefenseAction = (Result.ParryOutcome == EJoustParryOutcome::NotAttempted) ? EJoustDefenseAction::Guard : EJoustDefenseAction::Parry;

	Result.bIsEdgeParry = FJoustEdgeParryResolver::Resolve(Result.GuardZone, Result.ParryOutcome);

	Result.bBlockedScore = (Result.ParryOutcome == EJoustParryOutcome::Success);

	Result.CriticalDefenseMultiplier = Joust::Private::ResoveCriticalDefenseMultiplier(Result.GuardZone, Result.ParryOutcome, InRuleSet);

	return Result;
}
