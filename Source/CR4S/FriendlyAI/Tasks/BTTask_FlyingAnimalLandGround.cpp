#include "BTTask_FlyingAnimalLandGround.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "../BaseAnimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "../Component/FlyingMovementComponent.h"

UBTTask_FlyingAnimalLandGround::UBTTask_FlyingAnimalLandGround()
{
	NodeName = TEXT("FlyingAnimal: Task Land Ground");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_FlyingAnimalLandGround::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn) return EBTNodeResult::Failed;

	UFlyingMovementComponent* FlyComp = ControlledPawn->FindComponentByClass<UFlyingMovementComponent>();
	if (!FlyComp || !FlyComp->GroundTarget) return EBTNodeResult::Failed;
	
	FlyComp->ResetPitchAndRoll();

	return EBTNodeResult::InProgress;
}

void UBTTask_FlyingAnimalLandGround::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	UFlyingMovementComponent* FlyComp = ControlledPawn->FindComponentByClass<UFlyingMovementComponent>();
	if (!FlyComp || !FlyComp->GroundTarget)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	FlyComp->MoveToGroundTarget(DeltaSeconds);
	
	float DistSq = FVector::DistSquared(ControlledPawn->GetActorLocation(), FlyComp->GroundTarget->GetActorLocation());
	if (DistSq <= FMath::Square(LandDistanceThreshold))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}