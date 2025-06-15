#include "BTService_FAFlySkyTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "AIController.h"

UBTService_FAFlySkyTarget::UBTService_FAFlySkyTarget()
{
	NodeName = TEXT("Fly To Sky Target");
}

void UBTService_FAFlySkyTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControlledPawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!ControlledPawn) return;

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return;

	const float FlyingSpeed = BlackboardComp->GetValueAsFloat(TEXT("FlyingSpeed"));
	const float PitchAdjustment = BlackboardComp->GetValueAsFloat(TEXT("Pitch_Adjustment"));
	const float TurnSpeed = BlackboardComp->GetValueAsFloat(TEXT("FlyingTurnSpeed"));
	const FVector TargetLocation = BlackboardComp->GetValueAsVector(TEXT("NextSkyTarget"));
	
	FVector Offset = FVector(FlyingSpeed * DeltaSeconds, 0.f, 0.f);
	ControlledPawn->AddActorLocalOffset(Offset, true);

	FVector CurrentLocation = ControlledPawn->GetActorLocation();
	FRotator ActorRotation = ControlledPawn->GetActorRotation();
	FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();
	FVector LocalDirection = ActorRotation.UnrotateVector(Direction);

	float OffsetY = LocalDirection.Y;
	float OffsetZ = LocalDirection.Z;
	
	float TargetYawSpeed = OffsetY * TurnSpeed;
	CurrentYawSpeed = FMath::FInterpTo(CurrentYawSpeed, TargetYawSpeed, DeltaSeconds, 1.0f);

	float PitchTarget = OffsetZ * TurnSpeed;
	float YawInfluence = -0.2f * FMath::Abs(CurrentYawSpeed);
	float TargetPitchSpeed = PitchTarget + YawInfluence + PitchAdjustment;
	CurrentPitchSpeed = FMath::FInterpTo(CurrentPitchSpeed, TargetPitchSpeed, DeltaSeconds, 1.0f);

	float RollOptionA = -3.0f * ActorRotation.Roll;
	float RollOptionB = 0.5f * CurrentYawSpeed;
	float SelectedRoll = (FMath::Abs(OffsetY) > 0.5f) ? RollOptionA : RollOptionB;
	CurrentRollSpeed = FMath::FInterpTo(CurrentRollSpeed, SelectedRoll, DeltaSeconds, 1.0f);

	FRotator FinalRotation = FRotator(
		CurrentPitchSpeed * DeltaSeconds,
		CurrentYawSpeed * DeltaSeconds,
		CurrentRollSpeed * DeltaSeconds
	);
	ControlledPawn->AddActorLocalRotation(FinalRotation);
}

