#include "BTService_FAFlySkyTarget.h"
#include "AIController.h"
#include "../Component/FlyingMovementComponent.h"

UBTService_FAFlySkyTarget::UBTService_FAFlySkyTarget()
{
	NodeName = TEXT("Fly Toward Sky Target");
	bNotifyBecomeRelevant = true;
	bNotifyTick = true;
	Interval = 0.1f;
	RandomDeviation = 0.0f;
}

void UBTService_FAFlySkyTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* Pawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!Pawn) return;

	if (UFlyingMovementComponent* FlyingMove = Pawn->FindComponentByClass<UFlyingMovementComponent>())
	{
		FlyingMove->UpdateSkyTargetIfNear();
		FlyingMove->UpdateFlightTowardSkyTarget(DeltaSeconds);
	}
}
