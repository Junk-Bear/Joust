// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "JoustStrategyInput.generated.h"

UINTERFACE(MinimalAPI)
class UJoustStrategyInput : public UInterface
{
	GENERATED_BODY()
};

/**
 * Phase1 전략단계에서 사용할 입력제공자
 */
class JOUST_API IJoustStrategyInput
{
	GENERATED_BODY()

public:
	/** 선택한 전략 카드가 있으면 CardID를 반환한다. */
	virtual bool TryGetSelectedStrategyCardID(FName& OutCardID) const = 0;

	/** 봉인할 카드를 선택했으면 CardID를 반환한다. */
	virtual bool TryGetBannedStrategyCardID(FName& OutCardID) const = 0;
};
