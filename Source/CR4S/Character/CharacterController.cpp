#include "CharacterController.h"
#include "Game/CheatManager/C4CheatManager.h"

ACharacterController::ACharacterController():
	DefaultMappingContext(nullptr),
	MoveAction(nullptr),
	LookAction(nullptr),
	JumpAction(nullptr),
	SprintAction(nullptr)
{
	CheatClass = UC4CheatManager::StaticClass();
}
