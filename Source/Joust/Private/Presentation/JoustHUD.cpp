// Fill out your copyright notice in the Description page of Project Settings.


#include "Presentation/JoustHUD.h"
#include "Framework/JoustPlayerController.h"
#include "Presentation/JoustPresentationController.h"
#include "Presentation/JoustHUDWidget.h"

void AJoustHUD::BeginPlay()
{
	Super::BeginPlay();

	AJoustPlayerController* JoustPlayerController = Cast<AJoustPlayerController>(GetOwningPlayerController());

	if (JoustPlayerController == nullptr || !JoustPlayerController->IsLocalController())
		return;
	
	PresentationController = NewObject<UJoustPresentationController>(this);

	if (PresentationController == nullptr)
		return;
	
	if (!PresentationController->Initialize(JoustPlayerController, this))
	{
		PresentationController = nullptr;
	}
	
	if (!RootWidgetClass)
		return;
	
	RootWidget = CreateWidget<UJoustHUDWidget>(GetOwningPlayerController(), RootWidgetClass);

	if (IsValid(RootWidget))
	{
		RootWidget->AddToPlayerScreen();
	}
}
