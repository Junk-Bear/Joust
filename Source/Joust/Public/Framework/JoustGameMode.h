// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "JoustGameMode.generated.h"

class UJoustRuleSetDataAsset;

/**
 * 게임모드
 */
UCLASS()
class JOUST_API AJoustGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public: // 생성자용 블록
	AJoustGameMode();

protected:
	/** 이번 경기에서 사용할 전체 RuleSet */
	UPROPERTY(EditDefaultsOnly, Category = "Joust|Rules")
	TObjectPtr<UJoustRuleSetDataAsset> RuleSet = nullptr;

};
