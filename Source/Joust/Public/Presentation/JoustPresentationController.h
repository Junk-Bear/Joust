// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "JoustPresentationController.generated.h"

class AJoustGameState;
class AJoustHUD;
class AJoustPlayerController;
class AJoustPlayerState;

/**
 * Local Player의 Presentation 흐름을 중계
 */
UCLASS()
class JOUST_API UJoustPresentationController : public UObject
{
	GENERATED_BODY()
	
public: // ########## public 함수 블록 ############

	/** Local Presentation에 필요한 런타임 참조들 초기화 */
	bool Initialize(AJoustPlayerController* InPlayerController, AJoustHUD* InHUD);

	/** 현재 Local Player의 PlayerState 반환 */
	AJoustPlayerState* GetPlayerState() const;

private: // ########## private 변수 블록 ##########

	/** 이 Presentation을 소유한 Local PlayerController */
	TWeakObjectPtr<AJoustPlayerController> PlayerController;

	/** 모든 클라이언트가 읽는 공개 경기 상태 */
	TWeakObjectPtr<AJoustGameState> GameState;

	/** 실제 화면 표시를 담당하는 HUD */
	TWeakObjectPtr<AJoustHUD> HUD;

public: // ########## GET SET 블록 ##########

	FORCEINLINE AJoustPlayerController* GetPlayerController() const { return PlayerController.Get(); }

	FORCEINLINE AJoustGameState* GetGameState() const { return GameState.Get(); }

	FORCEINLINE AJoustHUD* GetHUD() const { return HUD.Get(); }

};
