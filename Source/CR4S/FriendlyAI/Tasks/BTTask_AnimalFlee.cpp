#include "BTTask_AnimalFlee.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/Character.h"
#include "../BaseAnimal.h"

UBTTask_AnimalFlee::UBTTask_AnimalFlee()
{
	NodeName = TEXT("AnimalFlee");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_AnimalFlee::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller) return EBTNodeResult::Failed;

	ABaseAnimal* Animal = Cast<ABaseAnimal>(Controller->GetPawn());
	if (!Animal) return EBTNodeResult::Failed;

	AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetActor"));
	if (!TargetActor) return EBTNodeResult::Failed;

	TargetToFlee = TargetActor;

	return EBTNodeResult::InProgress;
}

void UBTTask_AnimalFlee::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	ABaseAnimal* Animal = Cast<ABaseAnimal>(Controller ? Controller->GetPawn() : nullptr);
	
	if (!Animal)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (!TargetToFlee)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (!IsValid(TargetToFlee))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (Animal->CurrentState == EAnimalState::Dead)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	if (!Animal || !IsValid(TargetToFlee) || Animal->CurrentState == EAnimalState::Dead)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	FVector SelfLocation = Animal->GetActorLocation();
	FVector TargetLocation = TargetToFlee->GetActorLocation();
	float Distance = FVector::Dist(SelfLocation, TargetLocation);

	if (const FAnimalStatsRow* Stats = Animal->GetStatsRowPtr())
	{
		if (Animal->CurrentState != EAnimalState::Flee)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return;
		}
	}

	FVector AwayDirection = (SelfLocation - TargetLocation).GetSafeNormal();
	FVector FleeLocation = SelfLocation + AwayDirection * 600.f;

	FAIMoveRequest MoveReq;
	MoveReq.SetGoalLocation(FleeLocation);
	MoveReq.SetAcceptanceRadius(100.f);

	if (Controller)
	{
		Controller->MoveTo(MoveReq);
	}
}