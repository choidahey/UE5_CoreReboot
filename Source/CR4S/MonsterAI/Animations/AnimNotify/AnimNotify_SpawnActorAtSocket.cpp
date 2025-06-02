#include "MonsterAI/Animations/AnimNotify/AnimNotify_SpawnActorAtSocket.h"
#include "MonsterAI/Skills/BreathActor.h"
#include "MonsterAI/Skills/RollingActor.h"
#include "Kismet/GameplayStatics.h"

void UAnimNotify_SpawnActorAtSocket::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!IsValid(MeshComp) || !IsValid(SkillActorClass)) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!IsValid(Owner)) return;

	UWorld* World = Owner->GetWorld();
	if (!IsValid(World)) return;

	FVector SpawnLocation = MeshComp->GetSocketLocation(SocketName);
	FRotator SpawnRotation = MeshComp->GetSocketRotation(SocketName);

	FActorSpawnParameters Params;
	Params.Owner = Owner;
	Params.Instigator = Cast<APawn>(Owner);
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* SkillActor = World->SpawnActor<AActor>(SkillActorClass, SpawnLocation, SpawnRotation, Params);
	if (!SkillActor) return;

	SkillActor->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);

	if (!SkillActor->ActorHasTag(SkillActorTag))
	{
		SkillActor->Tags.Add(SkillActorTag);
	}					

	if (ABreathActor* Breath = Cast<ABreathActor>(SkillActor))
	{
		Breath->InitializeSkill(MeshComp, SocketName);
	}

	if (ARollingActor* Rolling = Cast<ARollingActor>(SkillActor))
	{
		SkillActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(Owner, 0);
		if (!PlayerPawn) return;
		
		const FVector Start = SkillActor->GetActorLocation();
		const FVector End = PlayerPawn->GetActorLocation();
		const FVector Direction = (End - Start).GetSafeNormal();

		Rolling->LaunchInDirection(Direction);
	}
}
