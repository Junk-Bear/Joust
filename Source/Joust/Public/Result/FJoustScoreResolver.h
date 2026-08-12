// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FJoustDefenseResult;

/**
 * 알맞는 점수를 반환하는 클래스
 */
class JOUST_API FJoustScoreResolver final
{
public:
	/** 점수를 반환 */
	static int32 Resolve(const FJoustDefenseResult& DefenseResult);
};
