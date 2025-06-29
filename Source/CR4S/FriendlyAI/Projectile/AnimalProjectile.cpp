#include "AnimalProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Game/System/ProjectilePoolSubsystem.h"
#include "../Component/ObjectPoolComponent.h"
#include "NiagaraComponent.h"
#include "Character/Characters/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "FriendlyAI/BaseAnimal.h"
#include "FriendlyAI/BaseHelperBot.h"
#include "Gimmick/GimmickObjects/DestructibleGimmick.h"
#include "Gimmick/GimmickObjects/Buildings/BaseBuildingGimmick.h"
#include "MonsterAI/BaseMonster.h"

AAnimalProjectile::AAnimalProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootSceneComponent;
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionObjectType(ECC_WorldDynamic);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	MeshComponent->SetNotifyRigidBodyCollision(true);
	MeshComponent->SetupAttachment(RootSceneComponent);
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	PoolComponent = CreateDefaultSubobject<UObjectPoolComponent>(TEXT("PoolComponent"));

	CapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollision"));
	CapsuleCollision->SetCapsuleRadius(100.f);
	CapsuleCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CapsuleCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	CapsuleCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CapsuleCollision->OnComponentBeginOverlap.AddDynamic(this, &AAnimalProjectile::OnCapsuleOverlap);
	CapsuleCollision->SetupAttachment(RootSceneComponent);

	HitEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("HitEffect"));
	HitEffect->SetupAttachment(RootSceneComponent);
	HitEffect->SetAutoActivate(false);
	HitEffect->Deactivate();
	HitEffect->OnSystemFinished.AddDynamic(this, &AAnimalProjectile::OnNiagaraFinished);

	ProjectileNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileNiagara"));
	ProjectileNiagaraComponent->SetupAttachment(RootSceneComponent);
	ProjectileNiagaraComponent->SetAutoActivate(true);
}

void AAnimalProjectile::BeginPlay()
{
	Super::BeginPlay();
    
	if (PoolComponent)
	{
		PoolComponent->OnReturnToPoolDelegate.AddDynamic(this, &AAnimalProjectile::ResetProjectile);
	}
}

void AAnimalProjectile::OnCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
										UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
										bool bFromSweep, const FHitResult& SweepResult)
{
	const ABaseAnimal* OwnerAnimal = Cast<ABaseAnimal>(GetOwner());
	if (!OwnerAnimal)
	{
		return;
	}

	if (bIgnoreSameSpeciesOverlap && !ShouldDamageActor(OtherActor))
	{
		return;
	}
	
	if (OtherActor && OtherActor != this
		&& OtherActor != GetOwner()
		&& !OtherActor->IsA<AAnimalProjectile>())
	{
		if (ProjectileMovement)
		{
			ProjectileMovement->StopMovementImmediately();
		}

		if (MeshComponent)
		{
			MeshComponent->SetVisibility(false);
		}

		if (ProjectileNiagaraComponent)
		{
			ProjectileNiagaraComponent->SetVisibility(false);
		}

		if (HitEffect)
		{
			HitEffect->SetWorldLocation(GetActorLocation());
			HitEffect->Activate();
		}

		if (CapsuleCollision)
		{
			CapsuleCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		// if (UWorld* World = GetWorld())
		// {
		// 	if (UProjectilePoolSubsystem* Pool = World->GetSubsystem<UProjectilePoolSubsystem>())
		// 	{
		// 		Pool->ReturnToPool(this);
		// 	}
		// }
	}
}
void AAnimalProjectile::ResetProjectile()
{
	if (HitEffect)
	{
		HitEffect->Deactivate();
	}
    
	if (MeshComponent)
	{
		MeshComponent->SetVisibility(true);
	}

	if (ProjectileNiagaraComponent)
	{
		ProjectileNiagaraComponent->SetVisibility(true);
	}

	if (CapsuleCollision)
	{
		CapsuleCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

void AAnimalProjectile::OnNiagaraFinished(UNiagaraComponent* PSystem)
{
	if (UWorld* World = GetWorld())
	{
		if (UProjectilePoolSubsystem* Pool = World->GetSubsystem<UProjectilePoolSubsystem>())
		{
			Pool->ReturnToPool(this);
		}
	}
}

bool AAnimalProjectile::ShouldDamageActor(AActor* OtherActor) const
{
	if (!OtherActor || OtherActor == this || OtherActor == GetOwner() || OtherActor->IsA<AAnimalProjectile>())
		return false;

	const ABaseAnimal* OwnerAnimal = Cast<ABaseAnimal>(GetOwner());
	if (!OwnerAnimal)
	{
		return false;
	}
	
	if (const ABaseAnimal* HitAnimal = Cast<ABaseAnimal>(OtherActor))
	{
		if (HitAnimal->RowName == OwnerAnimal->RowName)
			return false;
	}
	
	if (OtherActor->IsA<ADestructibleGimmick>())
	{
		return false;
	}

	if (OtherActor->IsA<ABaseBuildingGimmick>())
	{
		return bCanHitBuilding;
	}
	
	if (OtherActor->IsA<ABaseMonster>())
	{
		return false;
	}
	
	if (OtherActor->IsA<APlayerCharacter>())
	{
		return true;
	}
	
	if (OtherActor->IsA<ABaseHelperBot>())
	{
		return true;
	}

	return true;
}
