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
    Rampage = 2
};

UENUM(BlueprintType)
enum class EKamishAttackPattern : uint8
{
    None                        UMETA(DisplayName = "None"),

    // 개막
    Open_ThunderLeapSlash       UMETA(DisplayName = "Open, Thunder, Leap & Slash"),

    // 원거리 계열
    Ranged_ThrowDash            UMETA(DisplayName = "Throw, Dash"),
    Ranged_ThunderThrowDash     UMETA(DisplayName = "Thunder, Throw, Dash"),
    Ranged_ThunderLeap          UMETA(DisplayName = "Thunder, Leap"),
    Ranged_LongThunderLeap      UMETA(DisplayName = "Long Thunder, Leap"),

    // 중거리 계열
    Mid_LeapLight               UMETA(DisplayName = "Leap, Light Attack"),
    Mid_LeapHeavyLight          UMETA(DisplayName = "Leap, Heavy & Light"),
    Mid_DashHeavy               UMETA(DisplayName = "Strafe, Dash, Heavy"),
    Mid_ThrowDashHeavy          UMETA(DisplayName = "Throw, Dash, Heavy"),
    Mid_DashThrowDashHeavy      UMETA(DisplayName = "Dash, Throw, Dash, Heavy"),

    // 근거리 계열
    Melee_LightAttackA          UMETA(DisplayName = "Light Attack (Right Swing)"),
    Melee_LightAttackB          UMETA(DisplayName = "Light Attack (Left Swing)"),
    Melee_LightAttackC          UMETA(DisplayName = "Light Attack (Right Slam)"),
    Melee_LightAttackD          UMETA(DisplayName = "Light Attack (Left Upper)"),
    Melee_HeavyAttackA          UMETA(DisplayName = "Heavy Attack (Strike)"),
    Melee_HeavyAttackB          UMETA(DisplayName = "Heavy Attack (Left Swing)"),
    Melee_HeavyAttackC          UMETA(DisplayName = "Heavy Attack (Right Swing)"),
    Melee_Nanta                 UMETA(DisplayName = "Nanta")
};
