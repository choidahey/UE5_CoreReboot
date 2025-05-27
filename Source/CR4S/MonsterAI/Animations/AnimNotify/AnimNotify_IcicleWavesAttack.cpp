#include "MonsterAI/Animations/AnimNotify/AnimNotify_IcicleWavesAttack.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "MonsterAI/Skills/IcicleWavesActor.h"


UAnimNotify_IcicleWavesAttack::UAnimNotify_IcicleWavesAttack()
    : SpawnCount(3)
    , BaseDistance(500.f)
    , DistanceStep(800.f)
    , SpawnInterval(1.f)
    , bForward(true)
{
}

void UAnimNotify_IcicleWavesAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (!MeshComp || !IcicleWaveActorClass) return;

    ACharacter* OwnerChar = Cast<ACharacter>(MeshComp->GetOwner());
    if (!OwnerChar) return;

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
        SpawnActor->SpawnIcicleWaves(SpawnCount, BaseDistance, DistanceStep, SpawnInterval, bForward);
    }
}
