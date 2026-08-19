// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FJoustPredictionSeries;
struct FJoustPredictionSettings;

class IJoustRandomProvider;

/**
 * 가짜 예측원들을 만드는 클래스
 */
class JOUST_API FJoustFakePredictionGenerator final
{
public: // ########## public 함수 블록 ##########

	static bool Generate(
		const FJoustPredictionSettings& Settings,
		const FVector2D& AttackPoint,
		int32 PredictionSeed,
		const FVector2D& LanceBoxMin,
		const FVector2D& LanceBoxMax,
		int32 MaxRetries,
		float MinFakeAttackPointDistance,
		IJoustRandomProvider& RandomProvider,
		TArray<FJoustPredictionSeries>& OutFakeSeries);

private: // ########## private 함수 블록 ##########

	/** 
	* AttackPoint의 유효한 대칭점 후보를 생성 
	* 
	* X대칭, Y대칭, 대각대칭, 정중앙, 랜덤 총 5개 후보를 만듬
	*/	
	static bool GenerateFakeAttackPoint(
		const FVector2D& AttackPoint,
		const FVector2D& LanceBoxMin,
		const FVector2D& LanceBoxMax,
		int32 MaxRetries,
		float MinFakeAttackPointDistance,
		IJoustRandomProvider& RandomProvider,
		FVector2D& OutFakeAttackPoint);
};
