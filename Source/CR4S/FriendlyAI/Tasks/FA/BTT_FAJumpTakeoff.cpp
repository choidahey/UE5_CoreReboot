#include "BTT_FAJumpTakeoff.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTT_FAJumpTakeoff::UBTT_FAJumpTakeoff()
{
	NodeName = TEXT("Fly: Jump To Takeoff");
}

EBTNodeResult::Type UBTT_FAJumpTakeoff::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APawn* Pawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!Pawn) return EBTNodeResult::Failed;

	ACharacter* Character = Cast<ACharacter>(Pawn);
	if (!Character) return EBTNodeResult::Failed;
	
	Character->Jump();

	UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();
	if (!MoveComp) return EBTNodeResult::Failed;
	
	MoveComp->GravityScale = 0.1f;
	MoveComp->Velocity += Pawn->GetActorForwardVector() * 1400.0f + Pawn->GetActorUpVector() * 900.0f;

	return EBTNodeResult::Succeeded;
}
