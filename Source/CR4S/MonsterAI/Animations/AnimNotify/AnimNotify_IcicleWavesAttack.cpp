#include "MonsterAI/Animations/AnimNotify/AnimNotify_IcicleWavesAttack.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "MonsterAI/Skills/IcicleWavesActor.h"

void UAnimNotify_IcicleWavesAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (!IsValid(MeshComp)
        || !IsValid(MeshComp->GetWorld())
        || !IsValid(IcicleWaveActorClass)
        || !IsValid(Animation)) return;

    ACharacter* OwnerChar = Cast<ACharacter>(MeshComp->GetOwner());
    if (!IsValid(OwnerChar)) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = OwnerChar;
    SpawnParams.Instigator = OwnerChar->GetInstigator();
    FVector SpawnLocation = OwnerChar->GetActorLocation();

    bForward = FMath::RandBool();

    if (AIcicleWavesActor* SpawnActor = OwnerChar->GetWorld()->SpawnActor<AIcicleWavesActor>(
        IcicleWaveActorClass,
        SpawnLocation,
        FRotator::ZeroRotator,
        SpawnParams))
    {
        SpawnActor->InitIcicleWaves(SpawnCount, BaseDistance, DistanceStep, SpawnInterval, bForward);
    }
}
