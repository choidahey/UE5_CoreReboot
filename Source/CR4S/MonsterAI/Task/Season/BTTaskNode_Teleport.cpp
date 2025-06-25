#include "BTTaskNode_Teleport.h"
#include "AIController.h"
#include "CR4S.h"
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

    if (ACharacter* Character = Cast<ACharacter>(CachedPawn))
    {
        CachedMeshComp = Character->GetMesh();
    }
    else if (USkeletalMeshComponent* MeshComp = CachedPawn->FindComponentByClass<USkeletalMeshComponent>())
    {
        CachedMeshComp = MeshComp;
    }

    if (CachedMeshComp)
    {
        CachedMeshComp->SetVisibility(false, true);
    }

    if (UCapsuleComponent* CapsuleComp = CachedPawn->FindComponentByClass<UCapsuleComponent>())
        CapsuleComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
    
    if (TrailEffectTemplate && CachedMeshComp)
    {
        TrailEffectComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
            TrailEffectTemplate,
            CachedMeshComp,
            AttachBoneName,
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            EAttachLocation::KeepRelativeOffset,
            true,
            true,
            ENCPoolMethod::None,
            true);
    }
    
    if (BodyEffectTemplate && CachedMeshComp)
    {
        BodyEffectComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
            BodyEffectTemplate,
            CachedMeshComp,
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
        PathFollowComp->OnRequestFinished.RemoveAll(this);
        
        EPathFollowingRequestResult::Type MoveResult = AIC->MoveToLocation(Dest, AcceptanceRadius, false);
        
        if (MoveResult == EPathFollowingRequestResult::Failed)
        {
            return EBTNodeResult::Failed;
        }
        else if (MoveResult == EPathFollowingRequestResult::AlreadyAtGoal)
        {
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
            return EBTNodeResult::Succeeded;
        }
        
        TeleportRequestID = PathFollowComp->GetCurrentRequestId();
        PathFollowComp->OnRequestFinished.AddUObject(this, &UBTTaskNode_Teleport::OnMoveCompleted);
    }
    else
    {
        return EBTNodeResult::Failed;
    }

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

        if (UCharacterMovementComponent* MoveComp = Cast<UCharacterMovementComponent>(CachedPawn->GetMovementComponent()))
        {
            MoveComp->StopMovementImmediately();
            MoveComp->SetMovementMode(OriginalMovementMode);
            if (OriginalMovementMode == MOVE_Walking)
                MoveComp->MaxWalkSpeed = OriginalMaxSpeed;
            else
                MoveComp->MaxFlySpeed = OriginalMaxSpeed;
            MoveComp->MaxAcceleration = OriginalAcceleration;
        }

        if (IsValid(CachedMeshComp))
        {
            CachedMeshComp->SetVisibility(true, true);
        }
        
        if (UCapsuleComponent* CapsuleComp = CachedPawn->FindComponentByClass<UCapsuleComponent>())
            CapsuleComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
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
    if (!IsValid(CachedOwnerComp)) return;
    
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
            MoveComp->SetMovementMode(MOVE_Walking);
            
            if (OriginalMovementMode == MOVE_Walking)
                MoveComp->MaxWalkSpeed = OriginalMaxSpeed;
            else
                MoveComp->MaxFlySpeed  = OriginalMaxSpeed;
            MoveComp->MaxAcceleration = OriginalAcceleration;
        }
        
        if (IsValid(CachedMeshComp))
        {
            CachedMeshComp->SetVisibility(true, true);
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
    
    FinishLatentTask(*CachedOwnerComp.Get(), EBTNodeResult::Succeeded);
}
