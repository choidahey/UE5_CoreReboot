#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Engine/DataTable.h"
#include "FriendlyAI/Projectile/ObjectPoolable.h"
#include "ProjectilePoolSubsystem.generated.h"

class AObjectPoolable;

USTRUCT(BlueprintType)
struct FProjectileDataTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AObjectPoolable> BP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 PoolSize;
};

USTRUCT()
struct FPoolArray
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<AObjectPoolable*> ObjectPool;

	bool IsEmpty() const { return ObjectPool.IsEmpty(); }
	AObjectPoolable* Pop() { return ObjectPool.Pop(); }
	void Add(AObjectPoolable* ActorToAdd) { ObjectPool.Add(ActorToAdd); }
};

UCLASS()
class CR4S_API UProjectilePoolSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void SpawnFromPool(const TSubclassOf<AObjectPoolable> PoolClass, const FVector& Location, const FRotator& Rotation, AObjectPoolable*& SpawnedActor);
	void ReturnToPool(AObjectPoolable* Poolable);
	
	UFUNCTION(BlueprintCallable)
	void LoadProjectilePools();
	
private:
	
	//void LoadProjectilePools();
	void InitializePool(const TSubclassOf<AObjectPoolable>& PoolClass, int32 MaxSize, const FActorSpawnParameters& SpawnParams);
	AObjectPoolable* GetActorFromPool(const TSubclassOf<AObjectPoolable> PoolClass, const FVector& Location, const FRotator& Rotation);

	UPROPERTY()
	TMap<UClass*, FPoolArray> ObjectPools;
};