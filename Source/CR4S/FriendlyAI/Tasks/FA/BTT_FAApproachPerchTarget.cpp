#include "BTT_FAApproachPerchTarget.h"
#include "AIController.h"
#include "GameFramework/Actor.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"

UBTT_FAApproachPerchTarget::UBTT_FAApproachPerchTarget()
{
	NodeName = TEXT("Fly: Approach Perch Target");
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTT_FAApproachPerchTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APawn* ControlledPawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!ControlledPawn) return EBTNodeResult::Failed;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	StartLocation = ControlledPawn->GetActorLocation();
	StartRotation = ControlledPawn->GetActorRotation();

	TArray<AActor*> PerchTargets;
	UGameplayStatics::GetAllActorsWithTag(ControlledPawn->GetWorld(), FName("PerchTarget"), PerchTargets);
	if (PerchTargets.Num() == 0) return EBTNodeResult::Failed;

	AActor* Perch = PerchTargets[0];
	if (!Perch) return EBTNodeResult::Failed;

	FVector BaseLocation = Perch->GetActorLocation();
	float PerchOffset = BB->GetValueAsFloat("Perch_Offset");
	TargetLocation = FVector(BaseLocation.X, BaseLocation.Y, BaseLocation.Z + PerchOffset);

	TargetRotation = Perch->GetActorRotation();
	CurrentAlpha = 0.0f;

	return EBTNodeResult::InProgress;
}

void UBTT_FAApproachPerchTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	APawn* ControlledPawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!ControlledPawn) return;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;

	float Speed = BB->GetValueAsFloat("FlyToPerchSpeed");
	CurrentAlpha = FMath::FInterpConstantTo(CurrentAlpha, 1.1f, DeltaSeconds, Speed);

	FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation, CurrentAlpha);
	ControlledPawn->SetActorLocation(NewLocation);

	if (BB->GetValueAsBool("UsePerchArrow"))
	{
		FRotator NewRotation = FMath::Lerp(StartRotation, TargetRotation, CurrentAlpha);
		ControlledPawn->SetActorRotation(NewRotation);
	}

	if (CurrentAlpha >= 1.0f)
	{
		BB->SetValueAsBool("PlayPerchIdle", true);
		CurrentAlpha = 0.0f;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
