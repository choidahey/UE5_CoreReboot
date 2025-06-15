#include "ProjectilePoolSubsystem.h"
#include "GameFramework/Actor.h"

void UProjectilePoolSubsystem::SpawnFromPool(const TSubclassOf<AObjectPoolable> PoolClass, const FVector& Location, const FRotator& Rotation, AObjectPoolable*& SpawnedActor)
{
	SpawnedActor = GetActorFromPool(PoolClass, Location, Rotation);
}

void UProjectilePoolSubsystem::ReturnToPool(AObjectPoolable* Poolable)
{
	if (!Poolable) return;

	Poolable->OnReturnToPool();
	FPoolArray& Pool = ObjectPools.FindOrAdd(Poolable->GetClass());
	Pool.Add(Poolable);
}

void UProjectilePoolSubsystem::InitializePool(const TSubclassOf<AObjectPoolable>& PoolClass, int32 MaxSize, const FActorSpawnParameters& SpawnParams)
{
	FPoolArray& Pool = ObjectPools.FindOrAdd(PoolClass);
	for (int32 i = 0; i < MaxSize; ++i)
	{
		if (AObjectPoolable* NewActor = GetWorld()->SpawnActor<AObjectPoolable>(PoolClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams))
		{
			NewActor->OnReturnToPool();
			Pool.Add(NewActor);
		}
	}
}


void UProjectilePoolSubsystem::LoadProjectilePools()
{
	UE_LOG(LogTemp, Display, TEXT("Loading ProjectilePools"));
	static const FString ContextString(TEXT("ProjectilePoolInit"));
	UDataTable* ProjectileTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/CR4S/_Data/Projectile/ProjectileDataTable.ProjectileDataTable"));
	if (!ProjectileTable) return;
	
	TArray<FProjectileDataTableRow*> Rows;
	ProjectileTable->GetAllRows(ContextString, Rows);

	FActorSpawnParameters SpawnParams;
	for (const FProjectileDataTableRow* Row : Rows)
	{
		InitializePool(Row->BP, Row->PoolSize, SpawnParams);
	}
}

AObjectPoolable* UProjectilePoolSubsystem::GetActorFromPool(const TSubclassOf<AObjectPoolable> PoolClass, const FVector& Location, const FRotator& Rotation)
{
	FPoolArray& Pool = ObjectPools.FindOrAdd(PoolClass);

	if (!Pool.IsEmpty())
	{
		AObjectPoolable* Actor = Pool.Pop();
		Actor->SetActorLocationAndRotation(Location, Rotation);
		Actor->OnSpawnFromPool();
		return Actor;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (AObjectPoolable* NewActor = GetWorld()->SpawnActor<AObjectPoolable>(PoolClass, Location, Rotation, SpawnParams))
	{
		NewActor->OnSpawnFromPool();
		return NewActor;
	}

	return nullptr;
}
