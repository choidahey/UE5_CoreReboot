// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterController.h"

ACharacterController::ACharacterController():
	MenuAction(nullptr)
{
}

void ACharacterController::BeginPlay()
{
	SetInputMode(FInputModeGameOnly());
	bShowMouseCursor = false;
	Super::BeginPlay();
}

void ACharacterController::SetupInputComponent()
{
	Super::SetupInputComponent();

	//Bind Actions related UI
}
