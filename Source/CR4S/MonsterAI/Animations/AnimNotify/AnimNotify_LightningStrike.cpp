#include "MonsterAI/Animations/AnimNotify/AnimNotify_LightningStrike.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "MonsterAI/Skills/LightningStrikeActor.h"
#include "MonsterAI/Region/KamishForestBoss.h"
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

	FVector CloudOrigin = OwnerMonster->GetActorLocation() + FVector(0, 0, 1000.f);
	if (const AKamishForestBoss* Kamish = Cast<AKamishForestBoss>(OwnerMonster))
	{
		CloudOrigin = Kamish->GetCloudOriginLocation();
	}

	const float Angle = FMath::FRandRange(0.f, 2.f * PI);
	const FVector Offset = FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.f) * SpawnRadius;
	const FVector SpawnLocation = CloudOrigin + Offset;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ALightningStrikeActor* Lightning = World->SpawnActor<ALightningStrikeActor>(
		LightningStrikeClass, 
		SpawnLocation,
		FRotator::ZeroRotator,
		Params
	);

	if (Lightning)
	{
		Lightning->InitializeStrike(TargetLocation, LightningEffect, Damage);
	}
}
