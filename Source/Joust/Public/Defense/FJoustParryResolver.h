// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

enum class EJoustParryOutcome : uint8;
enum class EJoustGuardZone : uint8;

/**
 * 패링에 대한 성공여부 등을 계산하고 판별하는 클래스
 */
class JOUST_API FJoustParryResolver final
{
public:
	static EJoustParryOutcome Resolve(
		bool bInParryAttempted, EJoustGuardZone InGuardZone,
		float InParryInputTime, float InImpactTime, float InParrySense,
		float InPerfectParryWindow, float InGoodParryWindow, float InBadParryWindow,
		float& OutTimingError, float& OutAllowedParryWindow
	);
};
