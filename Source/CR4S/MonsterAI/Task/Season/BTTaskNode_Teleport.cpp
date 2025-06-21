#include "BTTaskNode_Teleport.h"
#include "AIController.h"
#include "NiagaraComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

UBTTaskNode_Teleport::UBTTaskNode_Teleport()
{
    NodeName = TEXT("Teleport Generic");
    bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTaskNode_Teleport::ExecuteTask(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory)
{
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) return EBTNodeResult::Failed;

    const FVector Dest = BB->GetValueAsVector(TeleportLocationKey.SelectedKeyName);
    AAIController* AIC = OwnerComp.GetAIOwner();
    if (!AIC) return EBTNodeResult::Failed;

    APawn* Pawn = AIC->GetPawn();
    if (!Pawn) return EBTNodeResult::Failed;
    
    UCharacterMovementComponent* MoveComp = Cast<UCharacterMovementComponent>(Pawn->GetMovementComponent());
    if (!MoveComp) return EBTNodeResult::Failed;

    CachedOwnerComp = &OwnerComp;
    CachedPawn = Pawn;
    
    OriginalMovementMode = MoveComp->MovementMode;
    if (OriginalMovementMode == MOVE_Walking)
        OriginalMaxSpeed = MoveComp->MaxWalkSpeed;
    else
        OriginalMaxSpeed = MoveComp->MaxFlySpeed;
    
    OriginalAcceleration = MoveComp->MaxAcceleration;
    
    MoveComp->SetMovementMode(MOVE_Flying);
    MoveComp->MaxFlySpeed = TeleportSpeed;
    MoveComp->MaxAcceleration = TeleportAcceleration;

    if (USkeletalMeshComponent* MeshComp = Cast<ACharacter>(CachedPawn)
        ? Cast<ACharacter>(CachedPawn)->GetMesh()
        : CachedPawn->FindComponentByClass<USkeletalMeshComponent>())
    {
        MeshComp->SetVisibility(false, true);
    }
    if (UCapsuleComponent* CapsuleComp = CachedPawn->FindComponentByClass<UCapsuleComponent>())
        CapsuleComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
    
    if (TrailEffectTemplate)
    {
        TrailEffectComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
            TrailEffectTemplate,
            CachedPawn->FindComponentByClass<USkeletalMeshComponent>(),
            AttachBoneName,
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            EAttachLocation::KeepRelativeOffset,
            true,
            true,
            ENCPoolMethod::None,
            true);
    }
    
    if (BodyEffectTemplate)
    {
        BodyEffectComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
            BodyEffectTemplate,
            CachedPawn->FindComponentByClass<USkeletalMeshComponent>(),
            AttachBoneName,
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            EAttachLocation::KeepRelativeOffset,
            true,
            true,
            ENCPoolMethod::None,
            true);
    }
    
    if (UPathFollowingComponent* PathFollowComp = AIC->GetPathFollowingComponent())
    {
        PathFollowComp->OnRequestFinished.AddUObject(this, &UBTTaskNode_Teleport::OnMoveCompleted);
    }
    
    AIC->MoveToLocation(Dest, AcceptanceRadius, false);

    return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTaskNode_Teleport::AbortTask(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory)
{
    if (IsValid(CachedPawn))
    {
        if (AAIController* AIC = Cast<AAIController>(CachedPawn->GetController()))
        {
            if (UPathFollowingComponent* PathFollowComp = AIC->GetPathFollowingComponent())
                PathFollowComp->OnRequestFinished.RemoveAll(this);
        }
    }
    
    if (TrailEffectComp)
    {
        TrailEffectComp->DestroyComponent();
        TrailEffectComp = nullptr;
    }
    if (BodyEffectComp)
    {
        BodyEffectComp->DestroyComponent();
        BodyEffectComp = nullptr;
    }

    return EBTNodeResult::Aborted;
}

void UBTTaskNode_Teleport::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    if (IsValid(CachedPawn))
    {
        if (AAIController* AIC = Cast<AAIController>(CachedPawn->GetController()))
        {
            if (UPathFollowingComponent* PathFollowComp = AIC->GetPathFollowingComponent())
                PathFollowComp->OnRequestFinished.RemoveAll(this);
        }
        
        if (UCharacterMovementComponent* MoveComp = Cast<UCharacterMovementComponent>(CachedPawn->GetMovementComponent()))
        {
            MoveComp->StopMovementImmediately();
            MoveComp->SetMovementMode(OriginalMovementMode);
            if (OriginalMovementMode == MOVE_Walking)
                MoveComp->MaxWalkSpeed = OriginalMaxSpeed;
            else
                MoveComp->MaxFlySpeed  = OriginalMaxSpeed;
            MoveComp->MaxAcceleration = OriginalAcceleration;
        }
        
        if (USkeletalMeshComponent* MeshComp = Cast<ACharacter>(CachedPawn)
            ? Cast<ACharacter>(CachedPawn)->GetMesh()
            : CachedPawn->FindComponentByClass<USkeletalMeshComponent>())
        {
            MeshComp->SetVisibility(true, true);
        }
        if (UCapsuleComponent* CapsuleComp = CachedPawn->FindComponentByClass<UCapsuleComponent>())
            CapsuleComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
    }
    
    if (IsValid(TrailEffectComp))
    {
        TrailEffectComp->DestroyComponent();
        TrailEffectComp = nullptr;
    }
    if (IsValid(BodyEffectComp))
    {
        BodyEffectComp->DestroyComponent();
        BodyEffectComp = nullptr;
    }
    
    if (IsValid(CachedOwnerComp))
    {
        FinishLatentTask(*CachedOwnerComp.Get(), EBTNodeResult::Succeeded);
    }
}
