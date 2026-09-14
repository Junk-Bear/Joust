// Fill out your copyright notice in the Description page of Project Settings.


#include "Defense/FJoustEdgeParryResolver.h"
#include "Common/JoustCommonTypes.h"

bool FJoustEdgeParryResolver::Resolve(EJoustGuardZone InGuardZone, EJoustParryOutcome InParryOutcome)
{
	return (InGuardZone == EJoustGuardZone::Bad && InParryOutcome == EJoustParryOutcome::Success);
}
