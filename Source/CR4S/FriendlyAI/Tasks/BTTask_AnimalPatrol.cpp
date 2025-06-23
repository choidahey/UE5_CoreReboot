#include "BTTask_AnimalPatrol.h"
#include "AIController.h"
#include "../Component/FlyingMovementComponent.h"
#include "../Component/GroundMovementComponent.h"
#include "../AnimalGround.h"
#include "../AnimalFlying.h"
#include "../BaseAnimal.h"
#include "../BaseAnimal.h"
#include "NavigationSystem.h"

UBTTask_AnimalPatrol::UBTTask_AnimalPatrol()
{
    NodeName = TEXT("Animal Patrol");
    bNotifyTick = true;
    bCreateNodeInstance = true;
}

uint16 UBTTask_AnimalPatrol::GetInstanceMemorySize() const
{
    return sizeof(FPatrolMemory);
}

EBTNodeResult::Type UBTTask_AnimalPatrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
    if (!IsValid(Pawn)) return EBTNodeResult::Failed;

    FPatrolMemory* Mem = (FPatrolMemory*)NodeMemory;
    Mem->Elapsed = 0.f;

    const FVector Origin = Pawn->GetActorLocation();
    const UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    if (!NavSys) return EBTNodeResult::Failed;

    FNavLocation Result;
    bool bFoundPoint = false;
    
    for (int32 i = 0; i < 50; ++i)
    {
        float CurrentRadius = PatrolRadius * FMath::Max(0.05f, 1.0f - (i * 0.02f));
        if (NavSys->GetRandomPointInNavigableRadius(Origin, CurrentRadius, Result))
        {
            bFoundPoint = true;
            break;
        }
    }
    
    if (!bFoundPoint)
    {
        return EBTNodeResult::Failed;
    }

    Mem->PatrolDestination = Result.Location;

    if (UFlyingMovementComponent* FlyComp = Pawn->FindComponentByClass<UFlyingMovementComponent>())
    {
        FlyComp->ResetPitchAndRoll();
        FlyComp->MoveToLocation(Mem->PatrolDestination);
        return EBTNodeResult::InProgress;
    }
    else if (UAnimalMovementComponent* MoveComp = Pawn->FindComponentByClass<UAnimalMovementComponent>())
    {
        MoveComp->MoveToLocation(Mem->PatrolDestination);
        return EBTNodeResult::InProgress;
    }
    
    return EBTNodeResult::Failed;
}

void UBTTask_AnimalPatrol::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
    if (!Pawn)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    FPatrolMemory* Mem = (FPatrolMemory*)NodeMemory;

    Mem->Elapsed += DeltaSeconds;
    if (Mem->Elapsed >= MaxWaitTime)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    bool bArrived = false;
    if (UFlyingMovementComponent* FlyComp = Pawn->FindComponentByClass<UFlyingMovementComponent>())
    {
        bArrived = FlyComp->HasReachedDestination(Mem->PatrolDestination, AcceptanceRadius);
    }

    else if (UGroundMovementComponent* GroundComp = Pawn->FindComponentByClass<UGroundMovementComponent>())
    {
        bArrived = GroundComp->HasReachedDestination(Mem->PatrolDestination, AcceptanceRadius);
    }

    if (bArrived)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}
