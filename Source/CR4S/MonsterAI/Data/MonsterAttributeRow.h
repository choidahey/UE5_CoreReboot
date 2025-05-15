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
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TSoftObjectPtr<UDataTable> SkillDataTable = nullptr;
};
