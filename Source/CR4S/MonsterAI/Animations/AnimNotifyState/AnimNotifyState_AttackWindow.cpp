#include "MonsterAI/Animations/AnimNotifyState/AnimNotifyState_AttackWindow.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "GameFramework/Character.h"

void UAnimNotifyState_AttackWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	ACharacter* OwningCharacter = Cast<ACharacter>(MeshComp->GetOwner());
	if (!OwningCharacter) return;

	UMonsterSkillComponent* SkillComp = OwningCharacter->FindComponentByClass<UMonsterSkillComponent>();
	SkillComp->SetAttackCollisionEnabled(true, SkillIndex);
}

void UAnimNotifyState_AttackWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	ACharacter* OwningCharacter = Cast<ACharacter>(MeshComp->GetOwner());
	if (!OwningCharacter) return;

	UMonsterSkillComponent* SkillComp = OwningCharacter->FindComponentByClass<UMonsterSkillComponent>();
	SkillComp->SetAttackCollisionEnabled(false, SkillIndex);
}
