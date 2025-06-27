#include "ObjectPoolComponent.h"
#include "Game/System/ProjectilePoolSubsystem.h"

UObjectPoolComponent::UObjectPoolComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UObjectPoolComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (bAutoRegisterOnBeginPlay && !bIsBeingInitialized)
	{
		if (UProjectilePoolSubsystem* PoolSubsystem = GetWorld()->GetSubsystem<UProjectilePoolSubsystem>())
		{
			PoolSubsystem->RegisterPoolableActor(GetOwner());
		}
	}
}

// Called when this actor is retrieved from the object pool and activated. Use OnSpawnFromPoolDelegate to bind custom initialization logic.
void UObjectPoolComponent::OnSpawnFromPool()
{
	OnSpawnFromPoolDelegate.Broadcast();
	
	GetOwner()->SetActorHiddenInGame(false);
	GetOwner()->SetActorEnableCollision(true);
}

void UObjectPoolComponent::OnReturnToPool()
{
	OnReturnToPoolDelegate.Broadcast();
	GetOwner()->SetActorHiddenInGame(true);
	GetOwner()->SetActorEnableCollision(false);
}

// Schedules this actor to be returned to the pool after a specified delay. If DelaySeconds is 0 or negative, returns to pool immediately.
void UObjectPoolComponent::ReturnToPoolAfter(float DelaySeconds = 0.0f)
{
	if (DelaySeconds <= 0.f)
	{
		if (UProjectilePoolSubsystem* Pool = GetWorld()->GetSubsystem<UProjectilePoolSubsystem>())
		{
			Pool->ReturnToPool(GetOwner());
		}
		return;
	}
	GetWorld()->GetTimerManager().SetTimer(ReturnTimerHandle,
		this, &UObjectPoolComponent::HandleReturnToPool,
		DelaySeconds, false);
}

// Internal function called by timer to return this actor to the pool. Do not call directly - use ReturnToPoolAfter() instead.
void UObjectPoolComponent::HandleReturnToPool()
{
	if (UProjectilePoolSubsystem* Pool = GetWorld()->GetSubsystem<UProjectilePoolSubsystem>())
	{
		Pool->ReturnToPool(GetOwner());
	}
}