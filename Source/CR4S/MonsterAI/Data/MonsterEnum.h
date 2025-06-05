#pragma once

#include "CoreMinimal.h"
#include "MonsterEnum.generated.h"

UENUM(BlueprintType)
enum class EMonsterState : uint8
{
    Idle        = 0  UMETA(DisplayName = "Idle"),
    Patrol      = 1  UMETA(DisplayName = "Patrol"),
    Chase       = 2  UMETA(DisplayName = "Chase"),
    Alert       = 3  UMETA(DisplayName = "Alert"),
    Combat      = 4  UMETA(DisplayName = "Combat"),
    Return      = 5  UMETA(DisplayName = "Return"),
    Attack      = 6  UMETA(DisplayName = "Attack"),
    Hit         = 7  UMETA(DisplayName = "Hit"),
    Casting     = 8  UMETA(DisplayName = "Casting"),
    Stunned     = 9  UMETA(DisplayName = "Stunned"),
    Dead        = 10 UMETA(DisplayName = "Dead"),
    AttackHouse = 11 UMETA(DisplayName = "AttackHouse"),
    MoveToHouse = 12 UMETA(DisplayName = "MoveToHouse")
};

UENUM(BlueprintType)
enum class EBossPhase : uint8
{
    Normal = 0,
    Berserk = 1,
    Groggy = 2
};
