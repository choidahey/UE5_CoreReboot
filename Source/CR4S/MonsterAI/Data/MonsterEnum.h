#pragma once

#include "CoreMinimal.h"
#include "MonsterEnum.generated.h"

UENUM(BlueprintType)
enum class EMonsterState : uint8
{
    Idle        UMETA(DisplayName = "Idle"),
    Patrol      UMETA(DisplayName = "Patrol"),
    Chase       UMETA(DisplayName = "Chase"),
    Alert       UMETA(DisplayName = "Alert"),
    Combat      UMETA(DisplayName = "Combat"),
    Return      UMETA(DisplayName = "Return"),
    Attack      UMETA(DisplayName = "Attack"),
    Hit         UMETA(DisplayName = "Hit"),
    Casting     UMETA(DisplayName = "Casting"),
    Stunned     UMETA(DisplayName = "Stunned"),
    Dead        UMETA(DisplayName = "Dead")
};
