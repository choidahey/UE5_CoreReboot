#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "AnimalProjectileStatsRow.generated.h"

class UProjectileMovementComponent;
class AAnimalProjectile;

USTRUCT(BlueprintType)
struct FAnimalProjectileStatsRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AAnimalProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Lifetime;
};