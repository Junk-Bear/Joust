// Fill out your copyright notice in the Description page of Project Settings.


#include "Defense/FJoustGuardResolver.h"
#include "Common/JoustCommonTypes.h"

EJoustGuardZone FJoustGuardResolver::Resolve(
	const FVector2D& InAttackPoint, const FVector2D& InShieldPoint, float InPerfectRadius, float InGoodRadius, float InBadRadius, float& OutHitDistance)
{
	OutHitDistance = static_cast<float>(FVector2D::Distance(InAttackPoint, InShieldPoint));

	if (OutHitDistance <= InPerfectRadius)
	{
		return EJoustGuardZone::Perfect;
	}

	if (OutHitDistance <= InGoodRadius)
	{
		return EJoustGuardZone::Good;
	}

	if (OutHitDistance <= InBadRadius)
	{
		return EJoustGuardZone::Bad;
	}

	return EJoustGuardZone::Outside;
}
