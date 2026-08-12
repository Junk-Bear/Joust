// Fill out your copyright notice in the Description page of Project Settings.


#include "Result/FJoustCriticalResolver.h"
#include "Interface/JoustRandomProvider.h"

bool FJoustCriticalResolver::Resolve(
	float BaseCriticalChance, float MaxCriticalChance, 
	float AttackTypeMultiplier, float DefenseMultiplier, float AttackerFinishing, float DefenderStability, 
	IJoustRandomProvider& RandomProvider, float& OutFinalCriticalChance)
{
	//최종 결정타 확률 계산
	const float RawCriticalChance = BaseCriticalChance * AttackTypeMultiplier * DefenseMultiplier * (1.0f + AttackerFinishing) * (1.0f - DefenderStability);

	OutFinalCriticalChance = FMath::Clamp(RawCriticalChance, 0.0f, MaxCriticalChance);

	//Perfect존 가드시 DefenseMultiplier = 0임 : 낙마 발생 판단을 안해도 됨.
	if (OutFinalCriticalChance <= 0.0f)
	{
		return false;
	}

	//낙마 발생했는지 롤을 돌리기
	const float Roll = RandomProvider.GetRandom(0.0f, 1.0f);

	return Roll < OutFinalCriticalChance;
}
