// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "JoustAttackInput.generated.h"

enum class EJoustAttackType : uint8;

UINTERFACE(MinimalAPI)
class UJoustAttackInput : public UInterface
{
	GENERATED_BODY()
};

/**
 * Phase2 공격 단계에서 사용할 입력제공자
 */
class JOUST_API IJoustAttackInput
{
	GENERATED_BODY()
	
public:
	/** 현재 공격 커서 위치를 제공 */
	virtual FVector2D GetAttackPoint() const = 0;

	/** 현재 선택한 찌르기 타입을 제공 */
	virtual EJoustAttackType GetAttackType() const = 0;

	/** 공격 선택이 최종 확정 되었는지 반환 */
	virtual bool IsAttackConfirmed() const = 0;
};
