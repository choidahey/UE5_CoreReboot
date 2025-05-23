#include "MonsterAI/Animations/AnimNotifyState/AnimNotifyState_ColdFairyAttack.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "MonsterAI/Skills/ColdFairyActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

UAnimNotifyState_ColdFairyAttack::UAnimNotifyState_ColdFairyAttack()
	: NumSpawnActor(10)
	, Interval(0.5f)
	, Damage(0.f)
    , SpawnRadius(500.f)
    , SpawnHeightOffset(300.f)
    , MyHeader(TEXT("NotifyState_ColdFairyAttack"))
{
}

void UAnimNotifyState_ColdFairyAttack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    if (!MeshComp || !ColdFairyActorClass || NumSpawnActor <= 0) return;

    AActor* Owner = MeshComp->GetOwner();
    if (!Owner) return;

    UWorld* World = Owner->GetWorld();
    if (!World) return;

    FVector Center = Owner->GetActorLocation() + FVector(0.f, 0.f, SpawnHeightOffset);
    float AngleStep = (NumSpawnActor > 1) ? 180.f / float(NumSpawnActor - 1) : 0.f;

    for (int32 i = 0; i < NumSpawnActor; ++i)
    {
        float AngleDeg = AngleStep * i;
        float AngleRad = FMath::DegreesToRadians(AngleDeg);

        FVector LocalDir = FVector(0.f, FMath::Cos(AngleRad), FMath::Sin(AngleRad));
        FVector WorldDir = Owner->GetActorRotation().RotateVector(LocalDir);

        FVector SpawnLoc = Center + WorldDir * SpawnRadius;
        FRotator SpawnRot = WorldDir.Rotation();
        SpawnRot.Pitch = 90.f;
        SpawnRot.Roll = 0.f;

        FActorSpawnParameters Params;
        Params.Owner = Owner;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        if (AColdFairyActor* Fairy = World->SpawnActor<AColdFairyActor>(
            ColdFairyActorClass,
            SpawnLoc,
            SpawnRot,
            Params))
        {
            SpawnedFairies.Add(Fairy);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[%s] Spawn failed idx=%d at %s"), *MyHeader, i, *SpawnLoc.ToString());
        }
    }

    World->GetTimerManager().SetTimer(
        LaunchTimerHandle,
        FTimerDelegate::CreateLambda([this, World]() {
                for (AColdFairyActor* Fairy : SpawnedFairies)
                {
                    if (IsValid(Fairy))
                        Fairy->Launch();
                }

                SpawnedFairies.Empty();
            }),
        1.0f,
        false
    );
}

void UAnimNotifyState_ColdFairyAttack::NotifyEnd(
    USkeletalMeshComponent* MeshComp,
    UAnimSequenceBase* Animation)
{
    if (MeshComp && MeshComp->GetWorld())
    {
        MeshComp->GetWorld()->GetTimerManager().ClearTimer(LaunchTimerHandle);
        SpawnedFairies.Empty();
    }
}
