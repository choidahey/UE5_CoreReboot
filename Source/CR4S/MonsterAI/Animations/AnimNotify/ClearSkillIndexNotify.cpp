#include "ClearSkillIndexNotify.h"
#include "CR4S/MonsterAI/Components/MonsterSkillComponent.h"
#include "CR4S/MonsterAI/BaseMonster.h"

void UClearSkillIndexNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (ABaseMonster* Monster = Cast<ABaseMonster>(MeshComp->GetOwner()))
	{
		if (UMonsterSkillComponent* SkillComponent = Monster->FindComponentByClass<UMonsterSkillComponent>())
		{
			SkillComponent->CurrentSkillIndex = INDEX_NONE;
			UE_LOG(LogTemp, Log, TEXT("[Notify] Cleared CurrentSkillIndex"));
		}
	}
}
