// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "JoustMatchStartWidget.generated.h"

class UButton;

/**
 * 경기 시작 및 참가자 대기 화면
 */
UCLASS()
class JOUST_API UJoustMatchStartWidget : public UUserWidget
{
	GENERATED_BODY()

public: // ########## 델리게이트 블록 ##########

	/** START MATCH 버튼 클릭 알림 */
	DECLARE_EVENT(UJoustMatchStartWidget, FOnStartMatchRequested); 

protected: // ########## protected 함수 블록 ##########

		virtual void NativeConstruct() override;

		virtual void NativeDestruct() override;

private: // ########## private 함수 블록 ##########

	/** START MATCH 버튼 클릭 처리 */
	UFUNCTION()
	void HandleStartMatchClicked();

private: // ########## private 변수 블록 ##########

	FOnStartMatchRequested StartMatchRequestedEvent;

private: // ########## Bind Widget 블록 ##########

	/** 경기 시작 요청 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_StartMatch = nullptr;

public: // ########## GET SET 블록 ##########

	FORCEINLINE FOnStartMatchRequested& OnStartMatchRequested()
	{
		return StartMatchRequestedEvent;
	}
};
