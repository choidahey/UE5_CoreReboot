#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EInventoryType : uint8
{
	Player UMETA(DisplayName = "플레이어"),
	ItemPouch UMETA(DisplayName = "아이템 주머니"),
	Storage UMETA(DisplayName = "창고")
};
