#pragma once

#include "CoreMinimal.h"
#include "Common/JoustCommonTypes.h"
#include "Defense/JoustDefenseTypes.h"
#include "JoustPlayerTypes.generated.h"

/**
 * 플레이어 스탯
 */
USTRUCT(BlueprintType)
struct JOUST_API FJoustPlayerStats
{
	GENERATED_BODY()

	/** 결정력 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Player")
	float Finishing = 0.0f;

	/** 기만 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Player")
	float Deception = 0.0f;

	/** 속공 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Player")
	float Quickness = 0.0f;

	/** 버팀 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Player")
	float Stability = 0.0f;

	/** 판독 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Player")
	float Reading = 0.0f;

	/** 방패 기동성 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Player")
	float ShieldMobility = 0.0f;

	/** 패링 감각 */
	UPROPERTY(BlueprintReadWrite, Category = "Joust|Player")
	float ParrySense = 0.0f;
};

/**
* Resolve가 끝난 과거의 공격 기록
* 
* 현재 진행 중인 공격은 기록X
*/
USTRUCT(BlueprintType)
struct JOUST_API FJoustAttackHistory
{
	GENERATED_BODY()

	/** 공격이 발생한 Round */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joust|Player")
	int32 RoundNumber = 0;

	/** 실제 AttackPoint */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joust|Player")
	FVector2D AttackPoint = FVector2D::ZeroVector;

	/** 사용한 찌르기 타입 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joust|Player")
	EJoustAttackType AttackType = EJoustAttackType::Normal;

	/** 당시 상대의 ShieldPoint */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joust|Player")
	FVector2D OpponentShieldPoint = FVector2D::ZeroVector;

	/** 당시 상대의 최종 방어 결과 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joust|Player")
	FJoustDefenseResult OpponentDefenseResult;

	/** 해당 공격으로 득점했는지 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joust|Player")
	bool bScored = false;

	/** 상대가 낙마했는지 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joust|Player")
	bool bOpponentUnhorsed = false;
};

/**
* Resolve가 끝난 과거의 수비 기록
* 
* 현재 진행 중인 수비는 기록X
*/
USTRUCT(BlueprintType)
struct JOUST_API FJoustDefenseHistory
{
	GENERATED_BODY()

	/** 수비가 발생한 Round */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joust|Player")
	int32 RoundNumber = 0;

	/** 상대의 실제 AttackPoint */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joust|Player")
	FVector2D OpponentAttackPoint = FVector2D::ZeroVector;

	/** 상대가 사용한 찌르기 타입 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joust|Player")
	EJoustAttackType OpponentAttackType = EJoustAttackType::Normal;

	/** 최종 ShieldPoint */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joust|Player")
	FVector2D ShieldPoint = FVector2D::ZeroVector;

	/** 최종 방어 판정 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joust|Player")
	FJoustDefenseResult DefenseResult;

	/** 해당 공격으로 실점했는지 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joust|Player")
	bool bConcededScore	= false;

	/** 낙마했는지 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joust|Player")
	bool bUnhorsed = false;
};