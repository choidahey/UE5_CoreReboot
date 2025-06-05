#include "AnimalRangedAttackComponent.h"
#include "../AnimalProjectileSubsystem.h"
#include "../Projectile/Manager/AnimalProjectilePoolManager.h"
#include "../Projectile/AnimalProjectile.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "../BaseAnimal.h"
#include "Components/ArrowComponent.h"
#include "Components/SceneComponent.h"

UAnimalRangedAttackComponent::UAnimalRangedAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAnimalRangedAttackComponent::BeginPlay()
{
	Super::BeginPlay();
	ProjectileSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UAnimalProjectileSubsystem>();
	PoolManager = UAnimalProjectilePoolManager::Get(GetWorld());
}

void UAnimalRangedAttackComponent::FireProjectile()
{
	if (!ProjectileSubsystem) return;

	const FAnimalProjectileInfo* Info = ProjectileSubsystem->GetProjectileInfo(ProjectileRowHandle.RowName);
	if (!Info) return;

	ABaseAnimal* OwnerAnimal = Cast<ABaseAnimal>(GetOwner());
	FTransform SpawnTransform = OwnerAnimal
		? OwnerAnimal->MuzzleArrow->GetComponentTransform()
		: GetOwner()->GetActorTransform();

	UWorld* World = GetWorld();
	if (!World) return;

	PoolManager = UAnimalProjectilePoolManager::Get(World);

	AAnimalProjectile* Projectile = nullptr;
	if (PoolManager)
	{
		Projectile = PoolManager->Acquire(Info->ProjectileClass);
	}
	if (!Projectile)
	{
		Projectile = World->SpawnActor<AAnimalProjectile>(
			Info->ProjectileClass,
			SpawnTransform.GetLocation(),
			SpawnTransform.GetRotation().Rotator()
		);
	}

	Projectile->SetActorTransform(SpawnTransform);
	Projectile->SetActorHiddenInGame(false);
	Projectile->SetActorEnableCollision(true);
	
	if (Projectile->ProjectileMovement)
	{
		Projectile->ProjectileMovement->Velocity = SpawnTransform.GetRotation().GetForwardVector() * Info->Speed;
	}
	Projectile->SetLifeSpan(Info->Lifetime);
}