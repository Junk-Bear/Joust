#pragma once

#include "CoreMinimal.h"
#include "JoustPredictionTypes.generated.h"

/**
* 이번 공격에서 실제로 사용할 Prediction 설정
* 
* RlueSet 기본값과
* AttackType, Deception, Reading 등의 보정을 모두 계산한
* 최종 실행용 데이터
*/
USTRUCT(BlueprintType)
struct JOUST_API FJoustPredictionSettings
{
	GENERATED_BODY()

	/** 최초 예측원 반지름 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Prediction")
	float InitialRadius = 0.0f;

	/** 전체 Prediction Stage 수*/
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Prediction")
	int32 StageCount = 0;

	/**
	* Stage 별 Radius 비율
	* 실제 Radius 계산에서 사용됨.
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Prediction")
	TArray<float> StageRadiusRatios;

	/** 
	* 각 Predicion Stage 별 축소 시간
	* 
	* StageDurations[i]의 뜻 :
	* Circle[i] -> Circle[i+1]로 축소되는 시간
	* (중심과 반지름을 보간하는데 까지 걸리는 시간)
	* 
	* 각 값은 0 보다 커야함 아니면 에러임.
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Prediction")
	TArray<float> StageDurations;

	/** 전체 Prediction 진행 시간 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Prediction")
	float TotalDuration = 0.0f;

	/** Trick Attack에서 사용할 Fake 원 수 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Prediction")
	int32 FakeCircleCount = 0;

	/** Fake Prediction이 제거되는 Stage */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Prediction")
	int32 FakeRemoveStage = INDEX_NONE;
};

/**
 * Prediction의 논리적인 원 하나에 대한 것.
 *
 * 실제 AttackPoint는 저장하지 않는다.
 * Real/Fake 식별자도 넣지 않는다.
 */
USTRUCT(BlueprintType)
struct JOUST_API FJoustPredictionCircle
{
	GENERATED_BODY()

	/** 원 중심 위치*/
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Prediction")
	FVector2D Center = FVector2D::ZeroVector;

	/** 원 반지름 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Prediction")
	float Radius = 0.0f;
};

/**
 * Phase 3 시작 전에 서버가 미리 생성하는
 * 전체 Prediction 원 경로.
 * 
 * 서버가 보관하는 미래 전체 경로
 */
USTRUCT(BlueprintType)
struct JOUST_API FJoustPredictionSeries
{
	GENERATED_BODY()

	/** 순서대로 재생할 Prediction Circle 목록 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Prediction")
	TArray<FJoustPredictionCircle> Circles;

	/** Sequence 생성에 사용한 Seed */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Prediction")
	int32 PredictionSeed = 0;
};

/** 
* 현재 화면에 실제 표시할 Prediction Circle 하나
* 
*/

USTRUCT(BlueprintType)
struct JOUST_API FJoustPredictionDisplayCircle
{
	GENERATED_BODY()

	/** 현재 화면에 표시할 원의 중심 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Prediction")
	FVector2D Center = FVector2D::ZeroVector;

	/** 현재 화면에 표시할 원의 반지름 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Prediction")
	float Radius = 0.0f;


};

/**
* 클라이언트 / HUD에 공개가능한
* 현재 Prediction Runtime 상태
*/
USTRUCT(BlueprintType)
struct JOUST_API FJoustPredictionState
{
	GENERATED_BODY()

	/** 현재 화면에 표시할 예측원 원의 목록*/
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Prediction")
	TArray<FJoustPredictionDisplayCircle> DisplayCircles;

	/** 현재 Stage */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Prediction")
	int32 CurrentStage = 0;

	/**
	* 현재 원 -> 다음 원 사이의 보간 진행도
	* 
	* 0.0 = 현재 원
	* 1.0 = 다음 원 도달
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Prediction")
	float TransitionAlpha = 0.0f;

	/** Prediction UI 표시 여부 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Prediction")
	bool bIsPredictionVisible = false;

	/** 실제 AttackPoint가 공개되었는지 여부 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Prediction")
	bool bIsAttackPointRevealed = false;

	/** 
	* 공개 이후의 실제 AttackPoint
	* 
	* bISAttackPointRevealed가 true일 때만 유효
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Prediction")
	FVector2D RevealedAttackPoint = FVector2D::ZeroVector;
};
