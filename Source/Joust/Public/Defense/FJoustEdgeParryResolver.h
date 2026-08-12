// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

enum class EJoustGuardZone : uint8;
enum class EJoustParryOutcome : uint8;

/**
 * 엣지 패링이 발동되었는지만 판단하는 클래스
 */
class JOUST_API FJoustEdgeParryResolver
{
public:
	/** BadZone에서 패링이 성공했는 판정함 */

	static bool Resolve(EJoustGuardZone GuardZone, EJoustParryOutcome ParryOutcome);
};
