// Fill out your copyright notice in the Description page of Project Settings.


#include "Defense/FJoustEdgeParryResolver.h"
#include "Common/JoustCommonTypes.h"

bool FJoustEdgeParryResolver::Resolve(EJoustGuardZone GuardZone, EJoustParryOutcome ParryOutcome)
{
	return (GuardZone == EJoustGuardZone::Bad && ParryOutcome == EJoustParryOutcome::Success)
}
