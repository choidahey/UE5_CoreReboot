#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MonsterSkillRow.generated.h"

USTRUCT(BlueprintType)
struct FMonsterSkillRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName SkillName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSoftObjectPtr<UAnimMontage> PreMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSoftObjectPtr<UAnimMontage> SkillMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Cooldown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Range;
};
