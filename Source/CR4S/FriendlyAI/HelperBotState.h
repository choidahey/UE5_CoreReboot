#pragma once

#include "HelperBotState.generated.h"

UENUM(BlueprintType)
enum class EHelperBotState : uint8
{
	Idle,
	Chopping,
	Mining,
	Gathering,
	Following,
	Defending,
	Repairing,
	Disabled,
	Fleeing
};