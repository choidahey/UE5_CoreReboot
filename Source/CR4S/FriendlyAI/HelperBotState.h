#pragma once

#include "HelperBotState.generated.h"

UENUM(BlueprintType)
enum class EHelperBotState : uint8
{
	Idle,
	ChopWood,
	Mining,
	Gathering,
	Following,
	Defending,
	Repairing,
	Disabled,
	Fleeing,
	Dead
};