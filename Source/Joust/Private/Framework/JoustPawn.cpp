// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/JoustPawn.h"
#include "Components/SceneComponent.h"

AJoustPawn::AJoustPawn()
{
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
}

