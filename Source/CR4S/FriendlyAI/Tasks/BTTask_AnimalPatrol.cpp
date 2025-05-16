#include "BTTask_AnimalPatrol.h"
#include "AIController.h"
#include "../Component/AnimalMovementComponent.h"
#include "../BaseAnimal.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

UBTTask_AnimalPatrol::UBTTask_AnimalPatrol()
{
    bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_AnimalPatrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    WaitTime    = FMath::FRandRange(IdleTimeMin, IdleTimeMax);
    WaitElapsed = 0.0f;
    bMoving     = false;
    return EBTNodeResult::InProgress;
}

void UBTTask_AnimalPatrol::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
    if (!Pawn)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }
    
    ABaseAnimal* Animal = Cast<ABaseAnimal>(OwnerComp.GetAIOwner()->GetPawn());
    if (!Animal || Animal->CurrentState != EAnimalState::Patrol)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }
    
    if (!bMoving)
    {
        WaitElapsed += DeltaSeconds;
        if (WaitElapsed < WaitTime)
        {
            return;
        }
        
        const FVector Origin = Pawn->GetActorLocation();
        const FVector Rand2D = UKismetMathLibrary::RandomUnitVector() * PatrolRadius;
        PatrolDestination = Origin + FVector(Rand2D.X, Rand2D.Y, 0.0f);
        
        if (UAnimalMovementComponent* MoveComp = Pawn->FindComponentByClass<UAnimalMovementComponent>())
        {
            MoveComp->MoveToLocation(PatrolDestination);
        }
        bMoving = true;
    }
    else
    {
        const float Distance = FVector::Dist(Pawn->GetActorLocation(), PatrolDestination);
        if (Distance <= 100.0f)
        {
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        }
    }
}
