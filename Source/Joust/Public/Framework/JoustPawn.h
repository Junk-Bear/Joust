// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "JoustPawn.generated.h"

class USceneComponent;

UCLASS()
class JOUST_API AJoustPawn : public APawn
{
	GENERATED_BODY()

public: // 생성사용 블록
	AJoustPawn();

protected:
	/** Joust Pawn 전체 Transform 기준점 */
	UPROPERTY(VisibleAnywhere, Category = "Joust|Components")
	TObjectPtr<USceneComponent> SceneRoot;

};
