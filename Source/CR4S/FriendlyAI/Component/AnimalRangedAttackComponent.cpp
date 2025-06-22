#include "AnimalRangedAttackComponent.h"

#include "AIController.h"
#include "ObjectPoolComponent.h"
#include "Game/System/ProjectilePoolSubsystem.h"
#include "FriendlyAI/Projectile/AnimalProjectile.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "../BaseAnimal.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

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

	UProjectilePoolSubsystem* PoolSubsystem = World->GetSubsystem<UProjectilePoolSubsystem>();
	if (!PoolSubsystem) return;

	AActor* SpawnedActor = PoolSubsystem->SpawnFromPool(ProjectileClass,
														SpawnTransform.GetLocation(),
														SpawnTransform.GetRotation().Rotator());
	
	AAnimalProjectile* Projectile = Cast<AAnimalProjectile>(SpawnedActor);
	if (!Projectile) return;

	if (auto PoolComp = Projectile->FindComponentByClass<UObjectPoolComponent>())
	{
		PoolComp->ReturnToPoolAfter(ProjectileLifetime);
	}

	AAIController* AICon = Cast<AAIController>(OwnerAnimal->GetController());
	UBlackboardComponent* BB = AICon ? AICon->FindComponentByClass<UBlackboardComponent>() : nullptr;
	AActor* Target = BB ? Cast<AActor>(BB->GetValueAsObject("TargetActor")) : nullptr;

	FVector Direction;
	if (Target)
	{
		FVector SpawnLoc = SpawnTransform.GetLocation();
		FRotator MuzzleRot = SpawnTransform.GetRotation().Rotator();
		FVector ToTarget = Target->GetActorLocation() - SpawnLoc;
		FRotator DesiredRot = ToTarget.Rotation();
		FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(DesiredRot, MuzzleRot);

		float ClampedYaw   = FMath::Clamp(DeltaRot.Yaw,   -MaxYawAngle,   MaxYawAngle);
		float ClampedPitch = FMath::Clamp(DeltaRot.Pitch, -MaxPitchAngle, MaxPitchAngle);

		Direction = (MuzzleRot + FRotator(ClampedPitch, ClampedYaw, 0)).Vector();
	}
	else
	{
		Direction = SpawnTransform.GetRotation().GetForwardVector();
	}

	if (Projectile->ProjectileMovement)
	{
		Projectile->ProjectileMovement->Velocity = Direction * ProjectileSpeed;
	}
}