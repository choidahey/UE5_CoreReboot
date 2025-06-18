#include "BTTask_FAFaceYaw.h"
#include "GameFramework/Actor.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"

UBTTask_FAFaceYaw::UBTTask_FAFaceYaw()
{
	NodeName = TEXT("Face Forward");
}

EBTNodeResult::Type UBTTask_FAFaceYaw::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	APawn* ControlledPawn = AICon->GetPawn();
	if (!ControlledPawn) return EBTNodeResult::Failed;

	FRotator Rotation = ControlledPawn->GetActorRotation();
	FRotator NewRotation = FRotator(0.f, Rotation.Yaw, 0.f);
	ControlledPawn->SetActorRotation(NewRotation);

	return EBTNodeResult::Succeeded;
}
