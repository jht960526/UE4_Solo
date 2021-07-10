// Fill out your copyright notice in the Description page of Project Settings.


#include "ABPlayerController.h"

void AABPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	UE_LOG(LogTemp,Warning,TEXT("Controller create"));
}

void AABPlayerController::OnPossess(APawn* aPawn)
{
	UE_LOG(LogTemp,Warning,TEXT("Possess Begin"));
	Super::OnPossess(aPawn);
}

void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}

