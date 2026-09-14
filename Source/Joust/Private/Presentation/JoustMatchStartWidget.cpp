// Fill out your copyright notice in the Description page of Project Settings.


#include "Presentation/JoustMatchStartWidget.h"
#include "Components/Button.h"

void UJoustMatchStartWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(Button_StartMatch))
	{
		Button_StartMatch->OnClicked.AddUniqueDynamic(this, &UJoustMatchStartWidget::HandleStartMatchClicked);
	}
}

void UJoustMatchStartWidget::NativeDestruct()
{
	if (IsValid(Button_StartMatch))
	{
		Button_StartMatch->OnClicked.RemoveDynamic(this, &UJoustMatchStartWidget::HandleStartMatchClicked);
	}

	Super::NativeDestruct();
}

void UJoustMatchStartWidget::HandleStartMatchClicked()
{
	StartMatchRequestedEvent.Broadcast();
}
