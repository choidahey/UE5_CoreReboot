#include "AnimalSingleTargetProjectile.h"

#include "FriendlyAI/BaseAnimal.h"
#include "Kismet/GameplayStatics.h"
#include "Game/System/ProjectilePoolSubsystem.h"
#include "Gimmick/GimmickObjects/DestructibleGimmick.h"

AAnimalSingleTargetProjectile::AAnimalSingleTargetProjectile()
{
}

void AAnimalSingleTargetProjectile::OnCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
													 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
													 bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnCapsuleOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (!ShouldDamageActor(OtherActor))
	{
		return;
	}

	UGameplayStatics::ApplyDamage(OtherActor, Damage, nullptr, this, UDamageType::StaticClass());
}
