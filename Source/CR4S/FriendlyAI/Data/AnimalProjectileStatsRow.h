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
	TSubclassOf<AAnimalProjectile> ProjectileClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Speed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Lifetime = 0.0f;
};