// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FJoustPredictionCircle;

/**
 * 예측원의 유효성 검증
 */
class JOUST_API FJoustPredictionValidator final
{
public: // ########### public 함수 블록 ##########

	/** 최초 예측원 검사 */
	static bool ValidateInitial( 
		const FJoustPredictionCircle& Circle, 
		const FVector2D& TargetPoint, 
		const FVector2D& LanceBoxMin, const FVector2D& LanceBoxMax);

	/** 다음 예측원 검사 */
	static bool ValidateNext(
		const FJoustPredictionCircle& CurrentCircle, const FJoustPredictionCircle& NextCircle,
		const FVector2D& TargetPoint,
		const FVector2D& LanceBoxMin, const FVector2D& LanceBoxMax);
	
private: // ########## private 함수 블록 ###########

	/** 랜스 박스 범위 자체 유효성 검사 */
	static bool ValidateLanceBox(const FVector2D& LanceBoxMin, const FVector2D& LanceBoxMax);

	/** Point의 유효검사 */
	static bool IsPointInLanceBox(const FVector2D& Point, const FVector2D& LanceBoxMin, const FVector2D& LanceBoxMax);

	/** 원 자체의 기본 유효성 검사 */
	static bool ValidateCircle(const FJoustPredictionCircle& Circle, const FVector2D& LanceBoxMin, const FVector2D& LanceBoxMax);

	/** 원이 Point를 포함하는지 검사 */
	static bool ContainsTargetPoint(const FJoustPredictionCircle& Circle, const FVector2D& TargetPoint);

	/** Inner Circle이 Outer Circle 내부인지 검사 */
	static bool ContainsCircle(const FJoustPredictionCircle& OuterCircle, const FJoustPredictionCircle& InnerCircle);
};
