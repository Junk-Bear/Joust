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
	float ResoveCriticalDefenseMultiplier(EJoustGuardZone GuardZone, EJoustParryOutcome ParryOutcome, const UJoustRuleSetDataAsset& RuleSet)
	{
		switch (ParryOutcome)
		{
			//패링 성공
		case EJoustParryOutcome::Success:
			return 0.0;

			//패링 실패시
		case EJoustParryOutcome::Failure:
			switch (GuardZone)
			{
			case EJoustGuardZone::Perfect:
				return RuleSet.ParryFailurePerfectCriticalMultiplier;

			case EJoustGuardZone::Good:
				return RuleSet.ParryFailureGoodCriticalMultiplier;

			case EJoustGuardZone::Bad:
				return RuleSet.ParryFailureBadCriticalMultiplier;

			case EJoustGuardZone::Outside:
			default:
				//아웃사이드 케이스 : 잘못된 경우의 수, 이미 ParryOutcom::NotAttempted여야 한다.
				ensureMsgf(false, TEXT("Outside cannot ParryOutcome-Failure"));

				return RuleSet.GuardOutsideCriticalMultiplier;
			}

			//패링 안함 => 가드판정에 따른 배율
		case EJoustParryOutcome::NotAttempted:
			switch (GuardZone)
			{
			case EJoustGuardZone::Perfect:
				return RuleSet.GuardPerfectCriticalMultiplier;

			case EJoustGuardZone::Good:
				return RuleSet.GuardGoodCriticalMultiplier;

			case EJoustGuardZone::Bad:
				return RuleSet.GuardBadCriticalMultiplier;

			case EJoustGuardZone::Outside:
			default:
				return RuleSet.GuardOutsideCriticalMultiplier;
			}

			//패링아웃컴이 없음 => 잘못된 경우의 수, 일단 체크 및 결정타 배율0.0
		default:
			ensureMsgf(false, TEXT("EJoustParryOutcome Invalid"));
			return 0.0f;
		}

	}
}

FJoustDefenseResult FJoustDefenseResolver::Resolve(
	const FJoustAttackData& AttackData, 
	const FJoustDefenseData& DefenseData, 
	float impactTime, 
	const UJoustRuleSetDataAsset& RuleSet)
{
	FJoustDefenseResult Result{};

	//Result 멤버변수 값 채우기
	Result.GuardZone = FJoustGuardResolver::Resolve(
		AttackData.AttackPoint, DefenseData.ShieldPoint,
		RuleSet.PerfectZoneRadius, RuleSet.GoodZoneRadius, RuleSet.BadZoneRadius,
		Result.HitDistance);

	Result.ParryOutcome = FJoustParryResolver::Resolve(
		DefenseData.bParryAttempted, Result.GuardZone,
		DefenseData.ParryInputTime, impactTime,
		DefenseData.ParrySense,
		RuleSet.PerfectZoneParryWindow, RuleSet.GoodZoneParryWindow, RuleSet.BadZoneParryWindow,
		Result.ParryTimingError, Result.AllowedParryWindow
	);

	Result.DefenseAction = (Result.ParryOutcome == EJoustParryOutcome::NotAttempted) ? EJoustDefenseAction::Guard : EJoustDefenseAction::Parry;

	Result.bIsEdgeParry = FJoustEdgeParryResolver::Resolve(Result.GuardZone, Result.ParryOutcome);

	Result.bBlockedScore = (Result.ParryOutcome == EJoustParryOutcome::Success);

	Result.CriticalDefenseMultiplier = Joust::Private::ResoveCriticalDefenseMultiplier(Result.GuardZone, Result.ParryOutcome, RuleSet);

	return Result;
}
