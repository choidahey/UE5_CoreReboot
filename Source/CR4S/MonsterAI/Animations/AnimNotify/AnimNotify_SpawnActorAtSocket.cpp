#include "MonsterAI/Animations/AnimNotify/AnimNotify_SpawnActorAtSocket.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "MonsterAI/Skills/BreathActor.h"
#include "MonsterAI/Skills/RollingActor.h"
#include "MonsterAI/Skills/FallingBombActor.h"
#include "MonsterAI/Skills/ProjectileSlashActor.h"
#include "MonsterAI/Skills/GroundShockActor.h"

void UAnimNotify_SpawnActorAtSocket::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!IsValid(MeshComp) || !IsValid(SkillActorClass)) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!IsValid(Owner)) return;

	UWorld* World = Owner->GetWorld();
	if (!IsValid(World)) return;

	for (int32 i = 0; i < SpawnCount; ++i)
	{
		if (bSpawnAllAtOnce)
		{
			SpawnSkillActor(MeshComp, Owner);
		}
		else
		{
			FTimerHandle TimerHandle;
			World->GetTimerManager().SetTimer(
				TimerHandle,
				FTimerDelegate::CreateUObject(this, &UAnimNotify_SpawnActorAtSocket::SpawnSkillActor, MeshComp, Owner),
				i * SpawnInterval,
				false
			);
		}
	}
}

void UAnimNotify_SpawnActorAtSocket::SpawnSkillActor(USkeletalMeshComponent* MeshComp, AActor* Owner)
{
	if (!MeshComp || !Owner || !SkillActorClass) return;

	UWorld* World = Owner->GetWorld();
	if (!World) return;

	FTransform BaseTransform = MeshComp->GetComponentTransform();
	if (SocketName != NAME_None && MeshComp->DoesSocketExist(SocketName))
	{
		BaseTransform = MeshComp->GetSocketTransform(SocketName, RTS_World);
	}

	const FVector SpawnLocation = BaseTransform.TransformPosition(SpawnOffset);
	const FRotator SpawnRotation = (BaseTransform.GetRotation().Rotator() + AdditionalRotation);

	FActorSpawnParameters Params;
	Params.Owner = Owner;
	Params.Instigator = Cast<APawn>(Owner);
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* SkillActor = World->SpawnActor<AActor>(SkillActorClass, SpawnLocation, SpawnRotation, Params);
	if (!SkillActor) return;

	SkillActor->SetActorScale3D(SpawnScale);
	SkillActor->SetActorRotation(SpawnRotation);

	if (MeshComp->DoesSocketExist(SocketName))
	{
		SkillActor->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
	}

	if (!SkillActor->ActorHasTag(SkillActorTag))
	{
		SkillActor->Tags.Add(SkillActorTag);
	}

	if (ABreathActor* Breath = Cast<ABreathActor>(SkillActor))
	{
		Breath->InitializeSkill(MeshComp, SocketName);
	}
	else if (ARollingActor* Rolling = Cast<ARollingActor>(SkillActor))
	{
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(Owner, 0);
		if (!IsValid(PlayerPawn)) return;

		const FVector Start = SkillActor->GetActorLocation();
		const FVector End = PlayerPawn->GetActorLocation();
		const FVector Direction = (End - Start).GetSafeNormal();

		Rolling->LaunchInDirection(Direction);
	}
	else if (AFallingBombActor* Bomb = Cast<AFallingBombActor>(SkillActor))
	{
		APawn* TargetPawn = UGameplayStatics::GetPlayerPawn(Owner, 0);
		Bomb->Initialize(Owner, TargetPawn);
	}
	else if (AProjectileSlashActor* Slash = Cast<AProjectileSlashActor>(SkillActor))
	{
		Slash->InitializeMovement(LaunchSpeed, TravelDistance, SpawnLocation);
	}
	else if (AGroundShockActor* Shock = Cast<AGroundShockActor>(SkillActor))
	{
		const FRotator FacingRotation = Owner->GetActorRotation();
		Shock->InitShock(FacingRotation);
	}
}
