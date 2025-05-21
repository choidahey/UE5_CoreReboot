// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterController.h"

ACharacterController::ACharacterController():
	DefaultMappingContext(nullptr),
	MoveAction(nullptr),
	LookAction(nullptr),
	JumpAction(nullptr),
	SprintAction(nullptr)
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
