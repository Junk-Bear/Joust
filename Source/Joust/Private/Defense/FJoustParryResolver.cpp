// Fill out your copyright notice in the Description page of Project Settings.


#include "Defense/FJoustParryResolver.h"
#include "Common/JoustCommonTypes.h"

EJoustParryOutcome FJoustParryResolver::Resolve(
	bool bInParryAttempted, EJoustGuardZone InGuardZone, 
	float InParryInputTime, float InImpactTime, float InParrySense, 
	float InPerfectParryWindow, float InGoodParryWindow, float InBadParryWindow, 
	float& OutTimingError, float& OutAllowedParryWindow)
{
	OutTimingError = 0.0f;
	OutAllowedParryWindow = 0.0f;

	//패링 안했을 경우 = 뒤의 계산 불필요
	if (!bInParryAttempted)
	{
		return EJoustParryOutcome::NotAttempted;
	}

	//Outside에서는 패링시도가 성립 안됨
	if (InGuardZone == EJoustGuardZone::Outside)
	{
		return EJoustParryOutcome::NotAttempted;
	}

	//가드 존에 따른 베이스패링윈도우 결정
	float BaseParryWindow = 0.0f;
	switch (InGuardZone)
	{
	case EJoustGuardZone::Perfect:
		BaseParryWindow = InPerfectParryWindow;
		break;

	case EJoustGuardZone::Good:
		BaseParryWindow = InGoodParryWindow;
		break;

	case EJoustGuardZone::Bad:
		BaseParryWindow = InBadParryWindow;
		break;

	default:
		return EJoustParryOutcome::NotAttempted;
	}

	//패링센스와 베이스패링윈도우를 합쳐 최종 패링허용 타이밍을 구함
	//패링 센스 0.1f = 패링 허용시간 + 10%
	const float ParrySenseMultiplier = FMath::Max(0.0f, 1.0f + InParrySense);

	OutAllowedParryWindow = BaseParryWindow * ParrySenseMultiplier;

	//Impact 시점과 입력 시간의 차이(절대값)
	OutTimingError = FMath::Abs(InParryInputTime - InImpactTime);

	if (OutTimingError <= OutAllowedParryWindow)
	{
		return EJoustParryOutcome::Success;
	}
	else
	{
		return EJoustParryOutcome::Failure;
	}
}
