// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterController.h"
#include "Game/System/TimeCheatManager.h"

ACharacterController::ACharacterController():
	DefaultMappingContext(nullptr),
	MoveAction(nullptr),
	LookAction(nullptr),
	JumpAction(nullptr),
	SprintAction(nullptr)
{
	CheatClass = UTimeCheatManager::StaticClass();
}
