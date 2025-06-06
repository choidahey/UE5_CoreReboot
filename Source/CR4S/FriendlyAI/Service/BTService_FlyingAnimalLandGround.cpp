#include "BTService_FlyingAnimalLandGround.h"
#include "AIController.h"
#include "../Component/FlyingMovementComponent.h"
#include "GameFramework/Pawn.h"

UBTService_FlyingAnimalLandGround::UBTService_FlyingAnimalLandGround()
{
	NodeName = TEXT("FlyingAnimal: Land When Close To GroundTarget");
	bNotifyBecomeRelevant = false;
	bNotifyCeaseRelevant = false;
	bCreateNodeInstance = true;
}

void UBTService_FlyingAnimalLandGround::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return;

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn) return;
	
	UFlyingMovementComponent* FlyComp = ControlledPawn->FindComponentByClass<UFlyingMovementComponent>();
	if (!FlyComp) return;
	
	AActor* GroundTarget = FlyComp->GroundTarget;
	if (!GroundTarget) return;
	
	const float DistSq = FVector::DistSquared(ControlledPawn->GetActorLocation(), GroundTarget->GetActorLocation());
	if (DistSq < FMath::Square(LandDistanceThreshold))
	{
		FlyComp->ResetPitchAndRoll();
		FlyComp->SetToWalkingMode();
	}
}
