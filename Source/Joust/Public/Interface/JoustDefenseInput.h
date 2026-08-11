// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "JoustDefenseInput.generated.h"

UINTERFACE(MinimalAPI)
class UJoustDefenseInput : public UInterface
{
	GENERATED_BODY()
};

/**
 * Phase 3에서 사용할 입력제공자
 */
class JOUST_API IJoustDefenseInput
{
	GENERATED_BODY()

public:

	/** 현재 방패 중심위치를 제공 */
	virtual FVector2D GetShieldPoint() const = 0;

	/** 이번 방어에서 패링입력을 했는지 반환 */
	virtual bool IsParryAttempted() const = 0;

	/** 패링 입력이 발생한 시간을 제공 */
	virtual float GetParryInputTime() const = 0;
};
