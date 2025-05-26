#include "MonsterAI/Animations/AnimNotify/AnimNotify_IceSpikeAttack.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "MonsterAI/Components/MonsterStateComponent.h"
#include "MonsterAI/Skills/IceSpike.h"

UAnimNotify_IceSpikeAttack::UAnimNotify_IceSpikeAttack()
	: MyHeader(TEXT("SpawnIceSpileSkill"))
{
}

void UAnimNotify_IceSpikeAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	APawn* OwnerPawn = Cast<APawn>(MeshComp->GetOwner());
	if (!OwnerPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] No valid OwnerPawn"), *MyHeader);
		return;
	}

	if (!IceSpikeActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] SpikeClass not set"), *MyHeader);
		return;
	}

	AIceSpike* Spike = AIceSpike::SpawnIceSpikeAtPlayer(OwnerPawn, IceSpikeActor);

	UE_LOG(LogTemp, Log, TEXT("[%s] UseSkill HiemsStrongAttack!"), *MyHeader);
}
