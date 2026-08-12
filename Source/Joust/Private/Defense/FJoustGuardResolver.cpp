// Fill out your copyright notice in the Description page of Project Settings.


#include "Defense/FJoustGuardResolver.h"
#include "Common/JoustCommonTypes.h"

EJoustGuardZone FJoustGuardResolver::Resolve(
	const FVector2D& AttackPoint, const FVector2D& ShieldPoint, float PerfectRadius, float GoodRadius, float BadRadius, float& OutHitDistance)
{
	OutHitDistance = static_cast<float>(FVector2D::Distance(AttackPoint, ShieldPoint));

	if (OutHitDistance <= PerfectRadius)
	{
		return EJoustGuardZone::Perfect;
	}

	if (OutHitDistance <= GoodRadius)
	{
		return EJoustGuardZone::Good;
	}

	if (OutHitDistance <= BadRadius)
	{
		return EJoustGuardZone::Bad;
	}

	return EJoustGuardZone::Outside;
}
