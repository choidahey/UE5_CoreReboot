#include "MonsterAI/Animations/AnimNotify/AnimNotify_ThrowProjectile.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/Skills/RotatingProjectile.h"
#include "MonsterAI/Skills/WeaponActor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotify_ThrowProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ABaseMonster* OwnerBoss = Cast<ABaseMonster>(MeshComp->GetOwner());
	if (!OwnerBoss || !ProjectileClass || !MeshComp->DoesSocketExist(AttachSocketName)) return;

	TArray<AActor*> AttachedActors;
	OwnerBoss->GetAttachedActors(AttachedActors);
	for (AActor* Attached : AttachedActors)
	{
		if (Attached && Attached->IsA(AWeaponActor::StaticClass()))
		{
			Attached->Destroy();
			break;
		}
	}

	FVector SpawnLocation = MeshComp->GetSocketLocation(AttachSocketName);
	FRotator SpawnRotation = MeshComp->GetSocketRotation(AttachSocketName);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.Owner = OwnerBoss;

	ARotatingProjectile* Projectile = MeshComp->GetWorld()->SpawnActor<ARotatingProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, Params);
	if (!Projectile) return;

	Projectile->SetActorScale3D(Projectile->ProjectileScale);

	AActor* PlayerPawn = UGameplayStatics::GetPlayerPawn(OwnerBoss, 0);
	if (!PlayerPawn) return;

	FVector TargetLocation = PlayerPawn->GetActorLocation();
	FHitResult HitResult;
	if (OwnerBoss->GetWorld()->LineTraceSingleByChannel(
		HitResult,
		TargetLocation + FVector(0, 0, 200),
		TargetLocation - FVector(0, 0, 500),
		ECC_Visibility))
	{
		TargetLocation = HitResult.Location;
	}

	Projectile->SetBossActor(OwnerBoss, AttachSocketName);
	Projectile->LaunchProjectile(TargetLocation, LaunchSpeed);
}
