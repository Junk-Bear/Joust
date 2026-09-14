// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

enum class EJoustGuardZone : uint8;

/**
 * 방어시 어느 존인지를 판별해주는 클래스
 */
class JOUST_API FJoustGuardResolver final
{
public:
	static EJoustGuardZone Resolve(
		const FVector2D& InAttackPoint,
		const FVector2D& InShieldPoint,
		float InPerfectRadius, float InGoodRadius, float InBadRadius, 
		float& OutHitDistance
	);
};
