#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MonsterSkillData.generated.h"

USTRUCT(BlueprintType)
struct FMonsterSkillData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName SkillName = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSoftObjectPtr<UAnimMontage> SkillMontage = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Damage = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Cooldown = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Range = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Stunt = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) uint8 bAllowMultipleHits : 1 = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) uint8 bUseWeaponCollision : 1 = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) uint8 bUseBodyCollision : 1 = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float BodyDamage = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float BodyStunt = 0;
};
