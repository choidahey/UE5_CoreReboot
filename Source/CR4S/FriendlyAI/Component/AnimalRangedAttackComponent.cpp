#include "AnimalRangedAttackComponent.h"
#include "Game/System/ProjectilePoolSubsystem.h"
#include "FriendlyAI/Projectile/AnimalProjectile.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "../BaseAnimal.h"

UAnimalRangedAttackComponent::UAnimalRangedAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAnimalRangedAttackComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UAnimalRangedAttackComponent::FireProjectile()
{
	if (!ProjectileClass) return;

	ABaseAnimal* OwnerAnimal = Cast<ABaseAnimal>(GetOwner());
	FTransform SpawnTransform = OwnerAnimal && OwnerAnimal->MuzzleArrow
		? OwnerAnimal->MuzzleArrow->GetComponentTransform()
		: GetOwner()->GetActorTransform();

	UWorld* World = GetWorld();
	if (!World) return;

	AAnimalProjectile* Projectile = nullptr;
	if (UProjectilePoolSubsystem* Pool = World->GetSubsystem<UProjectilePoolSubsystem>())
	{
		TSubclassOf<AObjectPoolable> BaseClass = ProjectileClass;
		AObjectPoolable* PooledObj = nullptr;
		Pool->SpawnFromPool(BaseClass,
							SpawnTransform.GetLocation(),
							SpawnTransform.GetRotation().Rotator(),
							PooledObj);
		Projectile = Cast<AAnimalProjectile>(PooledObj);
		if (!Projectile) return;
	}

	if (!Projectile) return;

	FTimerHandle ReturnHandle;
	World->GetTimerManager().SetTimer(ReturnHandle, FTimerDelegate::CreateWeakLambda(Projectile, [=]()
	{
		if (UProjectilePoolSubsystem* Pool = World->GetSubsystem<UProjectilePoolSubsystem>())
		{
			Pool->ReturnToPool(Projectile);
		}
	}), ProjectileLifetime, false);

	if (Projectile->ProjectileMovement)
	{
		Projectile->ProjectileMovement->Velocity = SpawnTransform.GetRotation().GetForwardVector() * ProjectileSpeed;
	}
}
