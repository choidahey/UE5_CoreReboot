#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MonsterSkillData.generated.h"


UENUM(BlueprintType)
enum class ESkillAttackType : uint8
{
	None = 0 UMETA(DisplayName="None"),
	Melee = 1 UMETA(DisplayName="Melee"),
	Ranged = 2 UMETA(DisplayName="Ranged"),
	Moving = 3 UMETA(DisplayName="Moving"),
};

// Probability structure based on Distance and Phase
USTRUCT(BlueprintType)
struct FSkillProbabilityEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinDistance = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxDistance = 9999.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Phase = 0; // 0 = Normal, 1 = Berserk, 2 = Groggy

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0.0, ClampMax = 1.0))
	float Probability = 1.f;
};

// Skill Data Structure
USTRUCT(BlueprintType)
struct FMonsterSkillData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName SkillName = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) ESkillAttackType AttackType = ESkillAttackType::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TObjectPtr<UAnimMontage> SkillMontage = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Damage = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Cooldown = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Range = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float StunGauge = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) uint8 bAllowMultipleHits : 1 = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) uint8 bUseWeaponCollision : 1 = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) uint8 bUseBodyCollision : 1 = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float BodyDamage = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float BodyStunt = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FSkillProbabilityEntry> ProbabilityList;
};
