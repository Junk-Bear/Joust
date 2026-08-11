// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Strategy/JoustStrategyTypes.h"
#include "JoustStrategyCardDataAsset.generated.h"

class UTexture2D;

/**
 * 전략 카드 한 장의 원본 데이터
 * 
 * 데이터만 보관
 */
UCLASS(BlueprintType)
class JOUST_API UJoustStrategyCardDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	/** 카드 내부 식별자 ID */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Strategy")
	FName CardID = NAME_None;

	/** UI 표시 이름 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Strategy")
	FText DisplayName;

	/** UI 카드 설명 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Strategy", meta = (MultiLine = "true"))
	FText Description;

	/** UI 카드 아이콘 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Strategy")
	TObjectPtr<UTexture2D> Icon = nullptr;

	/** 카드가 플레이어 능력치에 주는 변화량 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joust|Strategy")
	FJoustStrategyModifier Modifier;
};
