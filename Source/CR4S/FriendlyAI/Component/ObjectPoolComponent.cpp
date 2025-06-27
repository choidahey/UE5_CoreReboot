#include "ObjectPoolComponent.h"

#include "Character/Weapon/Bullet/BaseBullet.h"
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

// Called when this actor is retrieved from the pool and activated.
void UObjectPoolComponent::OnSpawnFromPool()
{
	GetOwner()->SetActorHiddenInGame(false);
	GetOwner()->SetActorEnableCollision(true);
}

// Schedule a delayed return to the pool
void UObjectPoolComponent::OnReturnToPool()
{
	GetWorld()->GetTimerManager().ClearTimer(ReturnTimerHandle);

	if (ABaseBullet* Bullet = Cast<ABaseBullet>(GetOwner()))
	{
		Bullet->Deactivate();
	}
	GetOwner()->SetActorHiddenInGame(true);
	GetOwner()->SetActorEnableCollision(false);
}
// Schedule a delayed return to the pool
void UObjectPoolComponent::ReturnToPoolAfter(float DelaySeconds)
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

void UObjectPoolComponent::HandleReturnToPool()
{
	if (UProjectilePoolSubsystem* Pool = GetWorld()->GetSubsystem<UProjectilePoolSubsystem>())
	{
		Pool->ReturnToPool(GetOwner());
	}
}