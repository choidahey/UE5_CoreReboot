#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MonsterAttributeRow.generated.h"

USTRUCT(BlueprintType)
struct FMonsterAttributeRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxHP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float AttackPower;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float AttackRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float AttackSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MoveSpeed;
};
