#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MonsterAttributeRow.generated.h"

USTRUCT(BlueprintType)
struct FMonsterAttributeRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName MonsterID = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxHP = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float AttackPower = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float AttackRange = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float AttackSpeed = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MoveSpeed = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxStun = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float StunningTime = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float StunDamageMultiplier = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float StunChargeStartDelay = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float StunRecoveryMin = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float StunRecoveryMax = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float StunRecoveryRampUpTime = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SightRadius = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float CombatRadius = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float LoseSightRadius = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float PeripheralVisionAngleDegrees = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float HearingRange = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TObjectPtr<UDataTable> SkillDataTable = nullptr;
};
