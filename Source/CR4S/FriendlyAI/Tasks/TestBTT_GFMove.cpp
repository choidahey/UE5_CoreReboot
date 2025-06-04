#include "TestBTT_GFMove.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/Character.h"
#include "../Component/FlyingMovementComponent.h"
#include "../Component/GroundMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

UTestBTT_GFMove::UTestBTT_GFMove()
{
    NodeName = TEXT("Test GF Move");
}

EBTNodeResult::Type UTestBTT_GFMove::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    APawn* Pawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
    if (!Pawn)
    {
        return EBTNodeResult::Failed;
    }

    if (UFlyingMovementComponent* FlyingMove = Pawn->FindComponentByClass<UFlyingMovementComponent>())
    {
        FlyingMove->BeginCruiseFromTakeOff();

        if (AActor* Target = FlyingMove->GetCurrentSkyTarget())
        {
            FlyingMove->MoveToActor(Target);
        }

        return EBTNodeResult::Succeeded;
    }
    return EBTNodeResult::Failed;
}