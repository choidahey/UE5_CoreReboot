#include "BTT_FASetToFlyingMode.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "../../Component/FlyingMovementComponent.h"
#include "FriendlyAI/AnimalFlying.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"


UBTT_FASetToFlyingMode::UBTT_FASetToFlyingMode()
{
	NodeName = TEXT("Fly: Set To Flying Mode");
}

EBTNodeResult::Type UBTT_FASetToFlyingMode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAnimalFlying* FlyingAnimal = Cast<AAnimalFlying>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		FlyingAnimal->SetMovementToFlying();
		UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
		BB->SetValueAsBool(TEXT("AreWeWalking"), false);
		BB->SetValueAsBool(TEXT("AreWePerched"), false);
		BB->SetValueAsBool(TEXT("IsLookingForPerch"), false);
		BB->SetValueAsBool(TEXT("DoADownTrace"), true);
		BB->SetValueAsBool(TEXT("AreWeFlying"), true);
		BB->SetValueAsBool(TEXT("PlayPerchedIdle"), false);

		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
