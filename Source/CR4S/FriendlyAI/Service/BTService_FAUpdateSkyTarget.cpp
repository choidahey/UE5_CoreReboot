#include "BTService_FAUpdateSkyTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Algo/RandomShuffle.h"
#include "FriendlyAI/Controller/FAAIController.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

UBTService_FAUpdateSkyTarget::UBTService_FAUpdateSkyTarget()
{
	NodeName = TEXT("Update Sky Target");
}

void UBTService_FAUpdateSkyTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard) return;

	AAIController* AIOwner = OwnerComp.GetAIOwner();
	if (!AIOwner) return;

	APawn* ControlledPawn = AIOwner->GetPawn();
	if (!ControlledPawn) return;

	AFAAIController* AIController = Cast<AFAAIController>(ControlledPawn->GetController());
	if (!AIController || AIController->FilteredSkyTargets.Num() == 0) return;

	const FVector CurrentLocation = ControlledPawn->GetActorLocation();
	const FVector CurrentTarget = OwnerComp.GetBlackboardComponent()->GetValueAsVector(FName("NextSkyTarget"));

	float Distance = FVector::Dist(CurrentLocation, CurrentTarget);

	if (Distance <= DistanceThreshold)
	{
		Algo::RandomShuffle(AIController->FilteredSkyTargets);
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(FName("NextSkyTarget"), AIController->FilteredSkyTargets[0]->GetActorLocation());
	}

}
