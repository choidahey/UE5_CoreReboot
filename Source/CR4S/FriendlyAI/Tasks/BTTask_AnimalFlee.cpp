#include "BTTask_AnimalFlee.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"
#include "../BaseAnimal.h"

UBTTask_AnimalFlee::UBTTask_AnimalFlee()
{
	NodeName = TEXT("AnimalFlee");
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_AnimalFlee::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller) return EBTNodeResult::Failed;

	ABaseAnimal* Animal = Cast<ABaseAnimal>(Controller->GetPawn());
	if (!Animal) return EBTNodeResult::Failed;

	AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetActor"));
	if (!TargetActor) return EBTNodeResult::Failed;

	if (ABaseAnimal* TargetAnimal = Cast<ABaseAnimal>(TargetActor))
	{
		if (TargetAnimal->RowName == Animal->RowName)
		{

			Animal->ClearTarget();
			if (UBlackboardComponent* BB = Controller->GetBlackboardComponent())
			{
				BB->ClearValue(TEXT("TargetActor"));
			}

			Animal->SetAnimalState(EAnimalState::Patrol);

			return EBTNodeResult::Failed;
		}
	}
	
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
	
	if (Animal->CurrentState == EAnimalState::Dead || Animal->CurrentState == EAnimalState::Stun)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	if (!IsValid(Animal) || Animal->IsActorBeingDestroyed() || !IsValid(TargetToFlee))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (ABaseAnimal* TargetAnimal = Cast<ABaseAnimal>(TargetToFlee))
	{
		if (TargetAnimal->RowName == Animal->RowName || TargetAnimal->CurrentState == EAnimalState::Dead)
		{
			Animal->ClearTarget();

			if (UBlackboardComponent* BB = Controller->GetBlackboardComponent())
			{
				BB->ClearValue(TEXT("TargetActor"));
			}

			Animal->SetAnimalState(EAnimalState::Patrol);

			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return;
		}
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
	
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSys)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	FNavLocation ProjectedLocation;
	bool bIsValid = NavSys->ProjectPointToNavigation(
		FleeLocation,
		ProjectedLocation,
		FVector(100.f, 100.f, 300.f)
	);

	if (!bIsValid)
	{

		Animal->ClearTarget();
		if (UBlackboardComponent* BB = Controller->GetBlackboardComponent())
		{
			BB->ClearValue(TEXT("TargetActor"));
		}
		Animal->SetAnimalState(EAnimalState::Patrol);

		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	FAIMoveRequest MoveReq;
	MoveReq.SetGoalLocation(ProjectedLocation.Location);
	MoveReq.SetAcceptanceRadius(100.f);

	if (Controller)
	{
		Controller->MoveTo(MoveReq);
	}
}