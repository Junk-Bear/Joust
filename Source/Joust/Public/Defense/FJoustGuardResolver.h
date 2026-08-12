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
	/** 
	* AttackPoint와 shieldPoint 사이의 거리를 계산하고
	* 알맞는 GuardZone을 반환
	*/
	static EJoustGuardZone Resolve(
		const FVector2D& AttackPoint,
		const FVector2D& ShieldPoint,
		float PerfectRadius, float GoodRadius, float BadRadius, 
		float& OutHitDistance
	);
};
