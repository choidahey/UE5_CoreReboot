#include "BTT_FAApproachPerchTarget.h"
#include "AIController.h"
#include "GameFramework/Actor.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "FriendlyAI/Controller/FAAIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"

UBTT_FAApproachPerchTarget::UBTT_FAApproachPerchTarget()
{
	NodeName = TEXT("Fly: Approach Perch Target");
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

uint16 UBTT_FAApproachPerchTarget::GetInstanceMemorySize() const
{
	return sizeof(FBTT_FAApproachPerchTargetMemory);
}


EBTNodeResult::Type UBTT_FAApproachPerchTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FBTT_FAApproachPerchTargetMemory* Memory = reinterpret_cast<FBTT_FAApproachPerchTargetMemory*>(NodeMemory);

	APawn* ControlledPawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!ControlledPawn) return EBTNodeResult::Failed;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	Memory->StartLocation = ControlledPawn->GetActorLocation();
	Memory->StartRotation = ControlledPawn->GetActorRotation();

	AFAAIController* FAAICon = Cast<AFAAIController>(OwnerComp.GetAIOwner());
	if (!FAAICon || FAAICon->FilteredPerchTargets.Num() == 0) return EBTNodeResult::Failed;

	AActor* Perch = FAAICon->FilteredPerchTargets[0];
	if (!Perch) return EBTNodeResult::Failed;

	FVector BaseLocation = Perch->GetActorLocation();
	float PerchOffset = BB->GetValueAsFloat("Perch_Offset");
	Memory->TargetLocation = FVector(BaseLocation.X, BaseLocation.Y, BaseLocation.Z + PerchOffset);

	Memory->TargetRotation = Perch->GetActorRotation();
	Memory->CurrentAlpha = 0.0f;

	return EBTNodeResult::InProgress;
}

void UBTT_FAApproachPerchTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FBTT_FAApproachPerchTargetMemory* Memory = reinterpret_cast<FBTT_FAApproachPerchTargetMemory*>(NodeMemory);

	APawn* ControlledPawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!ControlledPawn) return;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;

	float Speed = BB->GetValueAsFloat("FlyToPerchSpeed");
	Memory->CurrentAlpha = FMath::FInterpConstantTo(Memory->CurrentAlpha, 1.1f, DeltaSeconds, Speed);

	FVector NewLocation = FMath::Lerp(Memory->StartLocation, Memory->TargetLocation, Memory->CurrentAlpha);
	ControlledPawn->SetActorLocation(NewLocation);

	if (BB->GetValueAsBool("UsePerchArrow"))
	{
		FRotator LerpedRotation = FMath::Lerp(Memory->StartRotation, Memory->TargetRotation, Memory->CurrentAlpha);
		FRotator NewRotation = FRotator(LerpedRotation.Pitch, Memory->StartRotation.Yaw, LerpedRotation.Roll);
		ControlledPawn->SetActorRotation(NewRotation);
	}

	if (Memory->CurrentAlpha >= 1.0f)
	{
		BB->SetValueAsBool("PlayPerchIdle", true);
		Memory->CurrentAlpha = 0.0f;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
