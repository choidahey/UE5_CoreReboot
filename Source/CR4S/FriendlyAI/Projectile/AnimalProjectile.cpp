#include "AnimalProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "../Projectile/Manager/AnimalProjectilePoolManager.h"

AAnimalProjectile::AAnimalProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionObjectType(ECC_WorldDynamic);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Block);
	MeshComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	MeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	MeshComponent->SetNotifyRigidBodyCollision(true);
	MeshComponent->OnComponentHit.AddDynamic(this, &AAnimalProjectile::OnProjectileHit);
	RootComponent = MeshComponent;
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	
	Damage = 10.f;
}

void AAnimalProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AAnimalProjectile::OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
										UPrimitiveComponent* OtherComp, FVector NormalImpulse,
										const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this && !OtherActor->IsA<AAnimalProjectile>())
	{
		// TODO: Apply damage to OtherActor

		if (ProjectileMovement)
		{
			ProjectileMovement->StopMovementImmediately();
		}

		if (UWorld* World = GetWorld())
		{
			if (UAnimalProjectilePoolManager* Pool = UAnimalProjectilePoolManager::Get(World))
			{
				Pool->Release(this);
			}
		}
	}
}