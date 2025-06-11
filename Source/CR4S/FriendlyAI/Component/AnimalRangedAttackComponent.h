#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "AnimalRangedAttackComponent.generated.h"

class AAnimalProjectile;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UAnimalRangedAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAnimalRangedAttackComponent();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AAnimalProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere)
	float ProjectileSpeed = 0.f;

	UPROPERTY(EditAnywhere)
	float ProjectileLifetime = 0.f;

	UFUNCTION(BlueprintCallable)
	void FireProjectile();
};
