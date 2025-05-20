#include "MonsterAI/Animations/AnimNotify/AnimNotify_DeathCleanup.h"
#include "MonsterAI/BaseMonster.h"

void UAnimNotify_DeathCleanup::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (ABaseMonster* Monster = Cast<ABaseMonster>(MeshComp->GetOwner()))
    {
        SpawnDeathEffect(Monster);
        Monster->Destroy();
    }
}

void UAnimNotify_DeathCleanup::SpawnDeathEffect(AActor* Actor)
{
    if (!Actor)
    {
        return;
    }

    // TODO: Create Death Effect
    UE_LOG(LogTemp, Log, TEXT("[DeathSequence] Spawn fade-out effect."));
}
