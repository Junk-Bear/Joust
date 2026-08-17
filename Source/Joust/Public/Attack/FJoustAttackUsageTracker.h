// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Common/JoustCommonTypes.h"

class UJoustAttackTypeDataAsset;

/**
 * 경기 전체의 AttackType 사용횟수 관리
 */
class JOUST_API FJoustAttackUsageTracker
{
public: // ########### public 함수 블록 ##########

	/** AttackType 설정 기준으로 사용 횟수 초기화 */
	bool Initialize(const TMap<EJoustAttackType, TObjectPtr<UJoustAttackTypeDataAsset>>& AttackTypeSettings);

	/** 모든 사용 횟수 상태를 제거 */
	void Reset();

	/** 해당 AttackType을 현재 사용할 수 있는지 확인 */
	bool CanUse(EJoustAttackType AttackType) const;

	/** 실제 소비 */
	bool ConsumeUse(EJoustAttackType AttackType);

	/** 무제한 횟수 타입인지 확인 */
	bool IsUnlimited(EJoustAttackType AttackType) const;

	/** 남은 사용 횟수를 반환 (무제한 : INDEX_NONE, 미등록타입 : 0) */
	int32 GetRemainingUses(EJoustAttackType AttackType) const;

private: // ########## private 변수 블록 ##########

	/** 횟수(무제한 : INDEX_NONE) */
	TMap<EJoustAttackType, int32> RemainingUses;
};
