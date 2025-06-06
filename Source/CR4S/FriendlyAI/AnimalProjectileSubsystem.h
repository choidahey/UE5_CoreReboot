#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "Data/AnimalProjectileStatsRow.h"
#include "AnimalProjectileSubsystem.generated.h"

class AAnimalProjectile;
class UAnimalProjectilePoolManager;

USTRUCT()
struct FAnimalProjectileInfo
{
	GENERATED_BODY()

	UPROPERTY()
	TSubclassOf<AAnimalProjectile> ProjectileClass;

	UPROPERTY()
	float Speed;

	UPROPERTY()
	float Lifetime;
};

UCLASS()
class CR4S_API UAnimalProjectileSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	const FAnimalProjectileInfo* GetProjectileInfo(FName RowName) const;

	UPROPERTY()
	UAnimalProjectilePoolManager* PoolManager;
	
	FORCEINLINE UAnimalProjectilePoolManager* GetPoolManager() const { return PoolManager; }

	const TMap<FName, FAnimalProjectileInfo>& GetProjectileInfoMap() const { return ProjectileInfoMap; }

private:
	UPROPERTY()
	UDataTable* ProjectileDataTable;

	TMap<FName, FAnimalProjectileInfo> ProjectileInfoMap;
};