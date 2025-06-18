 #include "BTT_FAFlyLand.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "../../Component/FlyingMovementComponent.h"

UBTT_FAFlyLand::UBTT_FAFlyLand()
{
	NodeName = TEXT("Fly: Set To Walking Mode");
}

EBTNodeResult::Type UBTT_FAFlyLand::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APawn* Pawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!Pawn) return EBTNodeResult::Failed;

	if (UFlyingMovementComponent* FlyingMove = Pawn->FindComponentByClass<UFlyingMovementComponent>())
	{
		//FlyingMove->SetToWalkingMode();
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
