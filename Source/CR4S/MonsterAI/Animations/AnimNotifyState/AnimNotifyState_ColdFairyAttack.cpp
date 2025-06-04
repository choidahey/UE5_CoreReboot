#include "MonsterAI/Animations/AnimNotifyState/AnimNotifyState_ColdFairyAttack.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "AIController.h"
#include "CR4S.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "MonsterAI/Skills/ColdFairyActor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

void UAnimNotifyState_ColdFairyAttack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    if (!IsValid(MeshComp->GetWorld())
        || !IsValid(MeshComp)
        || !IsValid(SpawnActorClass)
        || !IsValid(Animation)
        || NumSpawnActor <= 0) return;
    
    APawn* OwnerPawn = Cast<APawn>(MeshComp->GetOwner());
    if (!CR4S_VALIDATE(LogDa, IsValid(OwnerPawn))) return;
    
    AAIController* AIC = Cast<AAIController>(OwnerPawn->GetController());
    if (!CR4S_VALIDATE(LogDa, IsValid(AIC))) return;
    
    UBlackboardComponent* BBComp = AIC->GetBlackboardComponent();
    if (!CR4S_VALIDATE(LogDa, IsValid(BBComp))) return;
    
    CurrentSpawnIndex = 0;
    SpawnedFairies.Empty();

    MeshComp->GetWorld()->GetTimerManager().SetTimer(
        SpawnTimerHandle,
        FTimerDelegate::CreateUObject(
            this,
            &UAnimNotifyState_ColdFairyAttack::SpawnOne,
            MeshComp,
            Cast<AAIController>(Cast<APawn>(MeshComp->GetOwner())->GetController()),
            Cast<AAIController>(Cast<APawn>(MeshComp->GetOwner())->GetController())->GetBlackboardComponent()
        ),
        Interval,
        true
    );
}

void UAnimNotifyState_ColdFairyAttack::SpawnOne(
    USkeletalMeshComponent* MeshComp,
    AAIController* AIC,
    UBlackboardComponent* BB)
{
    if (!MeshComp || !SpawnActorClass) return;

    APawn* OwnerPawn = Cast<APawn>(MeshComp->GetOwner());
    if (!OwnerPawn) return;

    int32 i = CurrentSpawnIndex;
    if (i < NumSpawnActor)
    {
        AActor* Target = Cast<AActor>(BB->GetValueAsObject(TEXT("TargetActor")));
        Target = Target ? Target : Cast<AActor>(BB->GetValueAsObject(TEXT("NearestHouseActor")));
        Target = Target ? Target : UGameplayStatics::GetPlayerPawn(MeshComp->GetWorld(), 0);

        FVector BaseLoc = Target->GetActorLocation();
        if (ACharacter* Char = Cast<ACharacter>(Target))
        {
            if (UCapsuleComponent* Cap = Char->GetCapsuleComponent())
            {
                BaseLoc.Z -= Cap->GetScaledCapsuleHalfHeight();
            }
        }
        
        FVector Center = OwnerPawn->GetActorLocation() + FVector(0.f, 0.f, SpawnHeightOffset);
        float AngleStep = (NumSpawnActor > 1) ? 180.f / float(NumSpawnActor - 1) : 0.f;
        float AngleRad = FMath::DegreesToRadians(AngleStep * i);
        
        FVector LocalDir = FVector(0.f, FMath::Cos(AngleRad), FMath::Sin(AngleRad));
        FVector WorldDir = OwnerPawn->GetActorRotation().RotateVector(LocalDir);

        FVector SpawnLoc = Center + WorldDir * SpawnRadius;
        FRotator SpawnRot = WorldDir.Rotation();
        SpawnRot.Roll = 0.f;
        
        FActorSpawnParameters Params;
        Params.Owner = OwnerPawn;
        Params.Instigator = OwnerPawn;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        
        AActor* SpawnActor = MeshComp->GetWorld()->SpawnActor<AActor>(
            SpawnActorClass,
            SpawnLoc,
            SpawnRot,
            Params
        );

        if (AColdFairyActor* FairyActor = Cast<AColdFairyActor>(SpawnActor))
        {
            SpawnedFairies.Add(FairyActor);
            FairyActor->InitialLaunch(Target, CurrentSpawnIndex, NumSpawnActor);
        }
        else
        {
            CR4S_Log(LogDa, Warning, TEXT("[%s] SpawnOne FAILED idx=%d"), *MyHeader, i);
        }

        CurrentSpawnIndex++;
    }
    else
    {
        MeshComp->GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
    }
}

void UAnimNotifyState_ColdFairyAttack::NotifyEnd(
    USkeletalMeshComponent* MeshComp,
    UAnimSequenceBase* Animation)
{
}
