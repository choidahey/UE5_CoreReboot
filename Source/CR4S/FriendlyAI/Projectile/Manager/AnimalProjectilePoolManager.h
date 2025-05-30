#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AnimalProjectilePoolManager.generated.h"

class AAnimalProjectile;

UCLASS()
class CR4S_API UAnimalProjectilePoolManager : public UObject
{
	GENERATED_BODY()

public:
	static UAnimalProjectilePoolManager* Get(UWorld* World);

	void InitializePool(int32 PoolSize);
	
	AAnimalProjectile* Acquire(TSubclassOf<AAnimalProjectile> ProjectileClass);
	void Release(AAnimalProjectile* Projectile);

private:
	TMap<TSubclassOf<AAnimalProjectile>, TArray<AAnimalProjectile*>> PoolMap;
};
