#include "BTService_FANearGroundAvoidOff.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "FriendlyAI/AnimalFlying.h"
#include "GameFramework/Pawn.h"
#include "FriendlyAI/Controller/FAAIController.h"
#include "Kismet/GameplayStatics.h"

UBTService_FANearGroundAvoidOff::UBTService_FANearGroundAvoidOff()
{
	NodeName = TEXT("Avoidance Off When Near Ground Target");
}

void UBTService_FANearGroundAvoidOff::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AFAAIController* AICon = Cast<AFAAIController>(OwnerComp.GetAIOwner());
	if (!AICon) return;

	APawn* ControlledPawn = AICon->GetPawn();
	if (!ControlledPawn) return;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;

	if (AICon->FilteredGroundTargets.Num() == 0) return;

	AActor* GroundTarget = AICon->FilteredGroundTargets[0];
	if (!GroundTarget) return;

	const float Distance = ControlledPawn->GetDistanceTo(GroundTarget);

	if (Distance <= 3000.f)
	{
		BB->SetValueAsBool("DoADownTrace", false);
	}
	else
	{
		AAnimalFlying* Animal = Cast<AAnimalFlying>(ControlledPawn);
		if (Animal && Animal->GetDoADownTrace())
		{
			BB->SetValueAsBool("DoADownTrace", true);
		}
	}
}
