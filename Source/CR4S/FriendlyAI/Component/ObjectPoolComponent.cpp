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

void UObjectPoolComponent::OnSpawnFromPool()
{
	GetOwner()->SetActorHiddenInGame(false);
	GetOwner()->SetActorEnableCollision(true);
}

void UObjectPoolComponent::OnReturnToPool()
{
	GetOwner()->SetActorHiddenInGame(true);
	GetOwner()->SetActorEnableCollision(false);
}