// Fill out your copyright notice in the Description page of Project Settings.


#include "Result/FJoustCriticalResolver.h"
#include "Interface/JoustRandomProvider.h"

bool FJoustCriticalResolver::Resolve(
	float InBaseCriticalChance, float InMaxCriticalChance, 
	float InAttackTypeMultiplier, float InDefenseMultiplier, float InAttackerFinishing, float InDefenderStability, 
	IJoustRandomProvider& InRandomProvider, float& OutFinalCriticalChance)
{
	//최종 결정타 확률 계산
	const float RawCriticalChance = InBaseCriticalChance * InAttackTypeMultiplier * InDefenseMultiplier * (1.0f + InAttackerFinishing) * (1.0f - InDefenderStability);

	OutFinalCriticalChance = FMath::Clamp(RawCriticalChance, 0.0f, InMaxCriticalChance);

	//Perfect존 가드시 DefenseMultiplier = 0임 : 낙마 발생 판단을 안해도 됨.
	if (OutFinalCriticalChance <= 0.0f)
	{
		return false;
	}

	//낙마 발생했는지 롤을 돌리기
	const float Roll = InRandomProvider.GetRandom(0.0f, 1.0f);

	return Roll < OutFinalCriticalChance;
}
