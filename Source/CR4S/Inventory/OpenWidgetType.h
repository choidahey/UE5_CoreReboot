#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EOpenWidgetType : uint8
{
	Player UMETA(DisplayName = "플레이어"),
	Storage UMETA(DisplayName = "창고"),
	ItemPouch UMETA(DisplayName = "아이템 주머니"),
	PlantBox UMETA(DisplayName = "밭"),
	RobotWorkshop UMETA(DisplayName = "로봇 정비소")
};
