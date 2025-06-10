#include "MonsterAI/Animations/AnimNotifyState/AnimNotifyState_ThrowProjectile.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/Skills/RotatingProjectile.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

void UAnimNotifyState_ThrowProjectile::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	ABaseMonster* OwnerBoss = Cast<ABaseMonster>(MeshComp->GetOwner());
	if (!OwnerBoss || !ProjectileClass) return;

	FVector SpawnLocation = MeshComp->GetSocketLocation(AttachSocketName);
	FRotator SpawnRotation = MeshComp->GetSocketRotation(AttachSocketName);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.Owner = OwnerBoss;

	ARotatingProjectile* Projectile = OwnerBoss->GetWorld()->SpawnActor<ARotatingProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, Params);
	if (!Projectile) return;

	Projectile->SetActorScale3D(Projectile->ProjectileScale);
	Projectile->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachSocketName);
	SpawnedProjectiles.Add(OwnerBoss, Projectile);
}

void UAnimNotifyState_ThrowProjectile::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	ABaseMonster* OwnerBoss = Cast<ABaseMonster>(MeshComp->GetOwner());
	if (!OwnerBoss) return;

	if (TWeakObjectPtr<ARotatingProjectile>* ProjectilePtr = SpawnedProjectiles.Find(OwnerBoss))
	{
		if (ARotatingProjectile* Projectile = ProjectilePtr->Get())
		{
			Projectile->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

			if (MeshComp->DoesSocketExist(AttachSocketName))
			{
				const FName BoneName = MeshComp->GetSocketBoneName(AttachSocketName);
				MeshComp->HideBoneByName(BoneName, PBO_None);
			}

			APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(OwnerBoss, 0);
			if (PlayerPawn)
			{
				UWorld* World = OwnerBoss->GetWorld();
				FVector PlayerLocation = PlayerPawn->GetActorLocation();
				FVector TraceStart = PlayerLocation + FVector(0, 0, 200.f);
				FVector TraceEnd = PlayerLocation - FVector(0, 0, 500.f);

				FHitResult HitResult;
				FCollisionQueryParams Params;
				Params.AddIgnoredActor(OwnerBoss);

				FVector TargetLocation = PlayerLocation;
				if (World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, Params))
				{
					TargetLocation = HitResult.Location;
				}

				Projectile->SetBossActor(OwnerBoss, AttachSocketName);
				Projectile->LaunchProjectile(TargetLocation, LaunchSpeed);

				DrawDebugLine(World, Projectile->GetActorLocation(), TargetLocation, FColor::Red, false, 1.f, 0, 3.f);
			}
		}
	}

	SpawnedProjectiles.Remove(OwnerBoss);
}
