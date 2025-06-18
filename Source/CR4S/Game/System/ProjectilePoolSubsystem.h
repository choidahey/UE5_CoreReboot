#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ProjectilePoolSubsystem.generated.h"

class UObjectPoolComponent;

USTRUCT()
struct FPoolArray
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<AActor*> ObjectPool;

	bool IsEmpty() const { return ObjectPool.IsEmpty(); }
	AActor* Pop() { return ObjectPool.Pop(); }
	void Add(AActor* ActorToAdd) { ObjectPool.Add(ActorToAdd); }
};

UCLASS()
class CR4S_API UProjectilePoolSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	AActor* SpawnFromPool(const TSubclassOf<AActor> ActorClass, const FVector& Location, const FRotator& Rotation);
	
	UFUNCTION(BlueprintCallable)
	void ReturnToPool(AActor* ActorToReturn);
	
	void RegisterPoolableActor(AActor* Actor);
	
private:
	void InitializePool(const TSubclassOf<AActor>& ActorClass, int32 PoolSize);
	AActor* GetActorFromPool(const TSubclassOf<AActor> ActorClass, const FVector& Location, const FRotator& Rotation);

	UPROPERTY()
	TMap<UClass*, FPoolArray> ObjectPools;
	
	UPROPERTY()
	TSet<UClass*> RegisteredClasses;
};