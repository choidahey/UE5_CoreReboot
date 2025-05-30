#include "BTTask_AnimalPatrol.h"
#include "AIController.h"
#include "../Component/AnimalMovementComponent.h"
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
    FNavLocation Result;

    const UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    if (!NavSys || !NavSys->GetRandomPointInNavigableRadius(Origin, PatrolRadius, Result))
    {
        return EBTNodeResult::Failed;
    }

    Mem->PatrolDestination = Result.Location;

    if (UAnimalMovementComponent* MoveComp = Pawn->FindComponentByClass<UAnimalMovementComponent>())
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

    const float Distance = FVector::Dist(Pawn->GetActorLocation(), Mem->PatrolDestination);
    if (Distance <= AcceptanceRadius)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}
