#include "MonsterAI/Animations/AnimNotify/AnimNotify_SpawnActorAtSocket.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "MonsterAI/Skills/BreathActor.h"
#include "MonsterAI/Skills/RollingActor.h"
#include "MonsterAI/Skills/FallingBombActor.h"

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

	FVector SpawnLocation = FVector::ZeroVector;
	FRotator SpawnRotation = FRotator::ZeroRotator;

	if (SocketName != NAME_None && MeshComp->DoesSocketExist(SocketName))
	{
		SpawnLocation = MeshComp->GetSocketLocation(SocketName);
		SpawnRotation = MeshComp->GetSocketRotation(SocketName);
	}
	else
	{
		SpawnLocation = MeshComp->GetComponentLocation();
		SpawnRotation = MeshComp->GetComponentRotation();
	}

	FActorSpawnParameters Params;
	Params.Owner = Owner;
	Params.Instigator = Cast<APawn>(Owner);
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* SkillActor = World->SpawnActor<AActor>(SkillActorClass, SpawnLocation, SpawnRotation, Params);
	if (!SkillActor) return;

	if (MeshComp->DoesSocketExist(SocketName))
	{
		SkillActor->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
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
}
