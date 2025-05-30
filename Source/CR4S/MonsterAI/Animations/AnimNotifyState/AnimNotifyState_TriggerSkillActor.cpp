#include "MonsterAI/Animations/AnimNotifyState/AnimNotifyState_TriggerSkillActor.h"

void UAnimNotifyState_TriggerSkillActor::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	TArray<AActor*> AttachedActors;
	Owner->GetAttachedActors(AttachedActors);

	for (AActor* Child : AttachedActors)
	{
		if (!Child || !Child->ActorHasTag(SkillTriggerTag)) continue;

		if (Child->GetClass()->FindFunctionByName(ActivateFunctionName))
		{
			Child->CallFunctionByNameWithArguments(*ActivateFunctionName.ToString(), *GLog, nullptr, true);
			UE_LOG(LogTemp, Log, TEXT("[TriggerSkillActor] Activated %s on %s"), *ActivateFunctionName.ToString(), *Child->GetName());
		}
	}
}

void UAnimNotifyState_TriggerSkillActor::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	TArray<AActor*> AttachedActors;
	Owner->GetAttachedActors(AttachedActors);

	for (AActor* Child : AttachedActors)
	{
		if (!Child || !Child->ActorHasTag(SkillTriggerTag)) continue;

		if (Child->GetClass()->FindFunctionByName(EndFunctionName))
		{
			Child->CallFunctionByNameWithArguments(*EndFunctionName.ToString(), *GLog, nullptr, true);
			UE_LOG(LogTemp, Log, TEXT("[TriggerSkillActor] Ended %s on %s"), *EndFunctionName.ToString(), *Child->GetName());
		}
		else
		{
			Child->Destroy();
		}
	}
}
