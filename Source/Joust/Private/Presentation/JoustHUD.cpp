// Fill out your copyright notice in the Description page of Project Settings.


#include "Presentation/JoustHUD.h"
#include "Framework/JoustPlayerController.h"
#include "Presentation/JoustPresentationController.h"
#include "Presentation/JoustHUDWidget.h"

void AJoustHUD::BeginPlay()
{
	Super::BeginPlay();

	AJoustPlayerController* JoustPlayerControllerPtr = Cast<AJoustPlayerController>(GetOwningPlayerController());

	if (!IsValid(JoustPlayerControllerPtr) || !JoustPlayerControllerPtr->IsLocalController() || !RootWidgetClass)
		return;
	
	RootWidget = CreateWidget<UJoustHUDWidget>(JoustPlayerControllerPtr, RootWidgetClass);

	if (!IsValid(RootWidget))
		return;

	PresentationController = NewObject<UJoustPresentationController>(this);

	if (!IsValid(PresentationController) || !PresentationController->Initialize(JoustPlayerControllerPtr, this, RootWidget))
	{
		PresentationController = nullptr;
		RootWidget = nullptr;

		return;
	}

	RootWidget->AddToPlayerScreen();
}
