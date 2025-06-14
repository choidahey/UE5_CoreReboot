#include "MonsterAI/Animations/AnimNotifyState/AnimNotifyState_FadeTeleport.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

void UAnimNotifyState_FadeTeleport::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    if (!MeshComp) return;

    AActor* Owner = MeshComp->GetOwner();
    if (!Owner) return;

    if (VanishEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(Owner, VanishEffect,
            Owner->GetActorLocation(), Owner->GetActorRotation(), FVector(1.f), true, true);

    }

    Owner->SetActorHiddenInGame(true);
    Owner->SetActorEnableCollision(false);
}

void UAnimNotifyState_FadeTeleport::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (!MeshComp) return;

    AActor* Owner = MeshComp->GetOwner();
    if (!Owner) return;

    APawn* Player = UGameplayStatics::GetPlayerPawn(Owner, 0);
    if (!Player) return;

    const FVector TargetBack = Player->GetActorLocation() - Player->GetActorForwardVector() * BackOffset;
    const FRotator TargetRot = (Player->GetActorLocation() - TargetBack).Rotation();

    Owner->SetActorLocation(TargetBack);
    Owner->SetActorRotation(TargetRot);

    if (AppearEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(Owner, AppearEffect,
            TargetBack, TargetRot, FVector(1.f), true, true);
    }

    Owner->SetActorHiddenInGame(false);
    Owner->SetActorEnableCollision(true);
}
