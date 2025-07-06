#include "AnimalExplosiveProjectile.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "FriendlyAI/BaseAnimal.h"

AAnimalExplosiveProjectile::AAnimalExplosiveProjectile()
{
	ExplosionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollision"));
	ExplosionCollision->SetupAttachment(RootSceneComponent);
	ExplosionCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ExplosionCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	ExplosionCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	ExplosionCollision->OnComponentBeginOverlap.AddDynamic(this, &AAnimalExplosiveProjectile::OnExplosionOverlap);

	ExplosionEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ExplosionEffect"));
	ExplosionEffect->SetupAttachment(RootSceneComponent);
	ExplosionEffect->SetAutoActivate(false);
	ExplosionEffect->Deactivate();
}

void AAnimalExplosiveProjectile::OnCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
												  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
												  bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA<AAnimalProjectile>())
	{
		return;
	}
	
	//UE_LOG(LogTemp, Warning, TEXT("Capsule Overlap with: %s"), OtherActor ? *OtherActor->GetName() : TEXT("NULL"));
	
	Super::OnCapsuleOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (ExplosionCollision)
	{
		ExplosionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

	if (ExplosionEffect)
	{
		ExplosionEffect->SetWorldLocation(GetActorLocation());
		ExplosionEffect->Activate();
	}

	if (bIsDotDamage)
	{
		GetWorldTimerManager().SetTimer(DotDamageTimerHandle, this, &AAnimalExplosiveProjectile::DealDotDamage, DotInterval, true);
	}

	GetWorldTimerManager().SetTimer(ExplosionTimerHandle, this, &AAnimalExplosiveProjectile::DeactivateExplosion, ExplosionDuration, false);
}

void AAnimalExplosiveProjectile::OnExplosionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
													UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
													bool bFromSweep, const FHitResult& SweepResult)
{
	if (!ShouldDamageActor(OtherActor))
	{
		return;
	}

	if (!bIsDotDamage)
	{
		if (DamagedActors.Contains(OtherActor))
		{
			return;
		}
		DamagedActors.Add(OtherActor);
	}

	float ExplosionDamage = Damage * ExplosionDamageMultiplier;
	UGameplayStatics::ApplyDamage(OtherActor, ExplosionDamage, nullptr, this, UDamageType::StaticClass());
}

void AAnimalExplosiveProjectile::DealDotDamage()
{
	if (!ExplosionCollision)
	{
		return;
	}

	TArray<AActor*> OverlappingActors;
	ExplosionCollision->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (ShouldDamageActor(Actor))
		{
			float ExplosionDamage = Damage * ExplosionDamageMultiplier;
			UGameplayStatics::ApplyDamage(Actor, ExplosionDamage, nullptr, this, UDamageType::StaticClass());
		}
	}
}

void AAnimalExplosiveProjectile::DeactivateExplosion()
{
	if (ExplosionCollision)
	{
		ExplosionCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (ExplosionEffect)
	{
		ExplosionEffect->Deactivate();
	}

	DamagedActors.Empty();
	GetWorldTimerManager().ClearTimer(DotDamageTimerHandle);
	GetWorldTimerManager().ClearTimer(ExplosionTimerHandle);
}