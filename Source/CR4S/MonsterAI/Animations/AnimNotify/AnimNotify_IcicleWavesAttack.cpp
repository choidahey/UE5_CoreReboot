#include "MonsterAI/Animations/AnimNotify/AnimNotify_IcicleWavesAttack.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "GameFramework/Character.h"

void UAnimNotify_IcicleWavesAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (!MeshComp) return;

    ACharacter* OwnerChar = Cast<ACharacter>(MeshComp->GetOwner());
    if (!OwnerChar) return;

    UMonsterSkillComponent* SkillComp = OwnerChar->FindComponentByClass<UMonsterSkillComponent>();
    if (!SkillComp) return;

    const FMonsterSkillData& Data = SkillComp->GetCurrentSkillData();
}
