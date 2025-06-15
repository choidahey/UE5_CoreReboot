#include "ProjectilePoolSubsystem.h"
#include "FriendlyAI/Component/ObjectPoolComponent.h"
#include "GameFramework/Actor.h"

AActor* UProjectilePoolSubsystem::SpawnFromPool(const TSubclassOf<AActor> ActorClass, const FVector& Location, const FRotator& Rotation)
{
	if (!ActorClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ObjectPool] SpawnFromPool: ActorClass is null"));
		return nullptr;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ObjectPool] SpawnFromPool: World is null"));
		return nullptr;
	}
	
	if (!RegisteredClasses.Contains(ActorClass))
	{
		AActor* TempActor = World->SpawnActorDeferred<AActor>(ActorClass, FTransform::Identity, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		
		if (TempActor)
		{
			if (UObjectPoolComponent* PoolComponent = TempActor->FindComponentByClass<UObjectPoolComponent>())
			{
				PoolComponent->SetIsBeingInitialized(true);
			}
			
			TempActor->FinishSpawning(FTransform::Identity);
			
			if (UObjectPoolComponent* PoolComponent = TempActor->FindComponentByClass<UObjectPoolComponent>())
			{
				PoolComponent->SetIsBeingInitialized(false);
				
				int32 SafePoolSize = FMath::Max(0, PoolComponent->InitialPoolSize);
				if (SafePoolSize > 0)
				{
					InitializePool(ActorClass, SafePoolSize);
				}
				
				PoolComponent->OnReturnToPool();
				FPoolArray& Pool = ObjectPools.FindOrAdd(ActorClass);
				Pool.Add(TempActor);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[ObjectPool] Actor doesn't have ObjectPoolComponent: %s"), *ActorClass->GetName());
				TempActor->Destroy();
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[ObjectPool] Failed to spawn temp actor: %s"), *ActorClass->GetName());
		}
		RegisteredClasses.Add(ActorClass);
	}

	return GetActorFromPool(ActorClass, Location, Rotation);
}

void UProjectilePoolSubsystem::ReturnToPool(AActor* ActorToReturn)
{
	if (!ActorToReturn) 
	{
		return;
	}

	if (!IsValid(ActorToReturn))
	{
		return;
	}

	if (UObjectPoolComponent* PoolComponent = ActorToReturn->FindComponentByClass<UObjectPoolComponent>())
	{
		PoolComponent->OnReturnToPool();
		FPoolArray& Pool = ObjectPools.FindOrAdd(ActorToReturn->GetClass());
		Pool.Add(ActorToReturn);
	}
}

void UProjectilePoolSubsystem::RegisterPoolableActor(AActor* Actor)
{
	if (!Actor) 
	{
		return;
	}

	if (!IsValid(Actor))
	{
		return;
	}

	UClass* ActorClass = Actor->GetClass();
	if (!ActorClass)
	{
		return;
	}

	if (!RegisteredClasses.Contains(ActorClass))
	{
		if (UObjectPoolComponent* PoolComponent = Actor->FindComponentByClass<UObjectPoolComponent>())
		{
			int32 SafePoolSize = FMath::Max(0, PoolComponent->InitialPoolSize - 1);
			if (SafePoolSize > 0)
			{
				InitializePool(ActorClass, SafePoolSize);
			}
			RegisteredClasses.Add(ActorClass);
		}
	}
}

void UProjectilePoolSubsystem::InitializePool(const TSubclassOf<AActor>& ActorClass, int32 PoolSize)
{
	if (!ActorClass)
	{
		return;
	}

	if (PoolSize <= 0)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ObjectPool] InitializePool: World is null"));
		return;
	}

	FPoolArray& Pool = ObjectPools.FindOrAdd(ActorClass);
	
	for (int32 i = 0; i < PoolSize; ++i)
	{
		AActor* NewActor = World->SpawnActorDeferred<AActor>(ActorClass, FTransform::Identity, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		
		if (NewActor)
		{
			if (UObjectPoolComponent* PoolComponent = NewActor->FindComponentByClass<UObjectPoolComponent>())
			{
				PoolComponent->SetIsBeingInitialized(true);
			}
			
			NewActor->FinishSpawning(FTransform::Identity);
			
			if (UObjectPoolComponent* PoolComponent = NewActor->FindComponentByClass<UObjectPoolComponent>())
			{
				PoolComponent->SetIsBeingInitialized(false);
				PoolComponent->OnReturnToPool();
				Pool.Add(NewActor);
			}
			else
			{
				NewActor->Destroy();
			}
		}
	}
}

AActor* UProjectilePoolSubsystem::GetActorFromPool(const TSubclassOf<AActor> ActorClass, const FVector& Location, const FRotator& Rotation)
{
	if (!ActorClass)
	{
		return nullptr;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ObjectPool] GetActorFromPool: World is null"));
		return nullptr;
	}

	FPoolArray& Pool = ObjectPools.FindOrAdd(ActorClass);

	if (!Pool.IsEmpty())
	{
		AActor* Actor = Pool.Pop();
		if (IsValid(Actor))
		{
			Actor->SetActorLocationAndRotation(Location, Rotation);
			
			if (UObjectPoolComponent* PoolComponent = Actor->FindComponentByClass<UObjectPoolComponent>())
			{
				PoolComponent->OnSpawnFromPool();
			}
			return Actor;
		}
	}
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	if (AActor* NewActor = World->SpawnActor<AActor>(ActorClass, Location, Rotation, SpawnParams))
	{
		if (UObjectPoolComponent* PoolComponent = NewActor->FindComponentByClass<UObjectPoolComponent>())
		{
			PoolComponent->OnSpawnFromPool();
		}
		return NewActor;
	}

	return nullptr;
}