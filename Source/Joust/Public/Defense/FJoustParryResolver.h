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
	/** 패링 입력 여부와 타이밍을 판단 */
	static EJoustParryOutcome Resolve(
		bool bParryAttempted, EJoustGuardZone GuardZone,
		float ParryInputTime, float ImpactTime, float ParrySense,
		float PerfectParryWindow, float GoodParryWindow, float BadParryWindow,
		float& OutTimingError, float& OutAllowedParryWindow
	);
};
