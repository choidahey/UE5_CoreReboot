#include "AnimalSingleTargetProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Game/System/ProjectilePoolSubsystem.h"

AAnimalSingleTargetProjectile::AAnimalSingleTargetProjectile()
{
}

void AAnimalSingleTargetProjectile::OnCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
													 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
													 bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnCapsuleOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	if (OtherActor && OtherActor != this 
		&& OtherActor != GetOwner() 
		&& !OtherActor->IsA<AAnimalProjectile>())
	{
		UGameplayStatics::ApplyDamage(OtherActor, Damage, nullptr, this, UDamageType::StaticClass());
		
		// if (UWorld* World = GetWorld())
		// {
		// 	if (UProjectilePoolSubsystem* Pool = World->GetSubsystem<UProjectilePoolSubsystem>())
		// 	{
		// 		Pool->ReturnToPool(this);
		// 	}
		// }
	}
}