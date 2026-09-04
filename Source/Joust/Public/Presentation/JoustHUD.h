// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "JoustHUD.generated.h"

class UJoustPresentationController; 
class UJoustHUDWidget;

/**
 * Joust Local Player의 HUD 진입점
 */
UCLASS(Abstract)
class JOUST_API AJoustHUD : public AHUD
{
	GENERATED_BODY()

protected: // ########## protected 함수 블록 ##########

	virtual void BeginPlay() override;

private: // ########## private 변수 블록 ##########

	/** Local Player Presentation 흐름 관리 */
	UPROPERTY(Transient)
	TObjectPtr<UJoustPresentationController>PresentationController = nullptr;

	/** 실제 화면에 생성할 최상위 JoustHUDWidget 클래스 */
	UPROPERTY(EditDefaultsOnly, Category = "Joust|HUD")
	TSubclassOf<UJoustHUDWidget> RootWidgetClass;

	/** 로컬 플레이어 화면에 생성된 최상위 HudWidget */
	UPROPERTY(Transient)
	TObjectPtr<UJoustHUDWidget> RootWidget;

public: // ########## GET SET 블록 ##########

	FORCEINLINE UJoustPresentationController* GetPresentationController() const { return PresentationController; }
};
