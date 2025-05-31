#include "MonsterAI/Animations/AnimNotifyState/AnimNotifyState_ColdFairyAttack.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "MonsterAI/Skills/ColdFairyActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

void UAnimNotifyState_ColdFairyAttack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    if (!MeshComp || !ColdFairyActorClass || NumSpawnActor <= 0) return;

    APawn* OwnerPawn = Cast<APawn>(MeshComp->GetOwner());
    if (!OwnerPawn) return;

    AAIController* AIC = Cast<AAIController>(OwnerPawn->GetController());
    UBlackboardComponent* BB = AIC->GetBlackboardComponent();
    if (!AIC || !BB) return;
    
    AActor* Target = Cast<AActor>(BB->GetValueAsObject(FAIKeys::TargetActor));
    Target = Target ? Target : Cast<AActor>(BB->GetValueAsObject(FSeasonBossAIKeys::NearestHouseActor));
    Target = Target ? Target : UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    
    FVector BaseLoc = Target->GetActorLocation();
    if (ACharacter* Char = Cast<ACharacter>(Target))
    {
        if (UCapsuleComponent* Cap = Char->GetCapsuleComponent())
            BaseLoc.Z -= Cap->GetScaledCapsuleHalfHeight();
    }

    FVector Center = OwnerPawn->GetActorLocation() + FVector(0.f, 0.f, SpawnHeightOffset);
    float AngleStep = (NumSpawnActor > 1) ? 180.f / float(NumSpawnActor - 1) : 0.f;

    for (int32 i = 0; i < NumSpawnActor; ++i)
    {
        float AngleDeg = AngleStep * i;
        float AngleRad = FMath::DegreesToRadians(AngleDeg);

        FVector LocalDir = FVector(0.f, FMath::Cos(AngleRad), FMath::Sin(AngleRad));
        FVector WorldDir = OwnerPawn->GetActorRotation().RotateVector(LocalDir);

        FVector SpawnLoc = Center + WorldDir * SpawnRadius;
        FRotator SpawnRot = WorldDir.Rotation();
        SpawnRot.Roll = 0.f;

        FActorSpawnParameters Params;
        Params.Owner = OwnerPawn;
        Params.Instigator = OwnerPawn;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        if (AColdFairyActor* Fairy = OwnerPawn->GetWorld()->SpawnActor<AColdFairyActor>(
            ColdFairyActorClass,
            SpawnLoc,
            SpawnRot,
            Params))
        {
            SpawnedFairies.Add(Fairy);
            Fairy->Launch(Target);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[%s] Spawn failed idx=%d at %s"), *MyHeader, i, *SpawnLoc.ToString());
        }
    }
}

void UAnimNotifyState_ColdFairyAttack::NotifyEnd(
    USkeletalMeshComponent* MeshComp,
    UAnimSequenceBase* Animation)
{
}
