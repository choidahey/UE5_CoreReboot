#include "MonsterAI/Animations/AnimNotify/AnimNotify_LightningStrike.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "MonsterAI/Skills/LightningStrikeActor.h"
#include "Kismet/GameplayStatics.h"

void UAnimNotify_LightningStrike::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!LightningStrikeClass || !LightningEffect) return;

	ABaseMonster* OwnerMonster = Cast<ABaseMonster>(MeshComp->GetOwner());
	if (!OwnerMonster) return;

	UMonsterSkillComponent* SkillComp = OwnerMonster->FindComponentByClass<UMonsterSkillComponent>();
	if (!SkillComp) return;

	const float Damage = SkillComp->GetCurrentSkillData().Damage;

	APawn* TargetPlayer = UGameplayStatics::GetPlayerPawn(OwnerMonster, 0);
	if (!TargetPlayer) return;

	const FVector TargetLocation = TargetPlayer->GetActorLocation();
	
	UWorld* World = OwnerMonster->GetWorld();
	if (!World) return;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ALightningStrikeActor* Lightning = World->SpawnActor<ALightningStrikeActor>(
		LightningStrikeClass,
		TargetLocation,
		FRotator::ZeroRotator,
		Params
	);

	if (Lightning)
	{
		Lightning->InitializeStrike(TargetLocation, LightningEffect, Damage);
	}
}
