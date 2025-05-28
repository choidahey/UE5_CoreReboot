#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EInventoryType : uint8
{
	Player UMETA(DisplayName = "플레이어"),
	Storage UMETA(DisplayName = "창고"),
	PlantBox UMETA(DisplayName = "밭"),
	Greenhouse UMETA(DisplayName = "온실"),
	CompostBin UMETA(DisplayName = "퇴비통")
};
