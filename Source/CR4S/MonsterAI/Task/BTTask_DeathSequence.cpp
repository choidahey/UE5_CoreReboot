#include "MonsterAI/Task/BTTask_DeathSequence.h"
#include "MonsterAI/MonsterAIHelper.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/Components/MonsterAnimComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

UBTTask_DeathSequence::UBTTask_DeathSequence()
{
    NodeName = TEXT("DeathSequence");
}

EBTNodeResult::Type UBTTask_DeathSequence::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    ABaseMonster* Monster = MonsterAIHelper::GetControlledMonster(OwnerComp);
    if (!Monster)
    {
        return EBTNodeResult::Failed;
    }

    // Play Death Montage
    if (UMonsterAnimComponent* AnimComp = Monster->GetAnimComponent())
    {
        AnimComp->PlayDeathMontage();
    }

    // Disable Collision
    if (UCapsuleComponent* Capsule = Monster->GetCapsuleComponent())
    {
        Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    // TODO: Drop Item
    // Monster->DropItem();
    UE_LOG(LogTemp, Log, TEXT("[DeathSequence] GiveDropItem."));

    return EBTNodeResult::Succeeded;
}
