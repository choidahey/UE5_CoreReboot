#include "BTT_FASetToPerchedMode.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"

UBTT_FASetToPerchedMode::UBTT_FASetToPerchedMode()
{
	NodeName = TEXT("Fly: Set To Perched Mode");
}

EBTNodeResult::Type UBTT_FASetToPerchedMode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	APawn* ControlledPawn = AICon->GetPawn();
	if (!ControlledPawn) return EBTNodeResult::Failed;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject("NextSkyTarget"));
	if (!TargetActor) return EBTNodeResult::Failed;
	
	FAIMoveRequest MoveReq;
	MoveReq.SetGoalActor(TargetActor);
	MoveReq.SetAcceptanceRadius(300.f);
	FNavPathSharedPtr NavPath;

	FPathFollowingRequestResult MoveResult = AICon->MoveTo(MoveReq, &NavPath);

	if (MoveResult.Code != EPathFollowingRequestResult::RequestSuccessful)
	{
		return EBTNodeResult::Failed;
	}
	
	AICon->StopMovement();

	if (UCharacterMovementComponent* MoveComp = ControlledPawn->FindComponentByClass<UCharacterMovementComponent>())
	{
		MoveComp->StopMovementImmediately();
	}

	BB->SetValueAsBool("IsLookingForPerch", false);
	BB->SetValueAsBool("AreWePerched", true);
	BB->SetValueAsBool("FlyToPerch", false);
	BB->SetValueAsBool("AreWeWalking", false);
	BB->SetValueAsBool("AreWeFlying", false);

	return EBTNodeResult::Succeeded;
}
