#include "BTService_FAFlyGroundTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Actor.h"
#include "FriendlyAI/Controller/FAAIController.h"
#include "Kismet/GameplayStatics.h"

UBTService_FAFlyGroundTarget::UBTService_FAFlyGroundTarget()
{
	NodeName = TEXT("Fly To Ground Target");
}

void UBTService_FAFlyGroundTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AFAAIController* AICon = Cast<AFAAIController>(OwnerComp.GetAIOwner());
	if (!AICon) return;

	APawn* ControlledPawn = AICon->GetPawn();
	if (!ControlledPawn) return;

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return;

	const float FlyingSpeed = BlackboardComp->GetValueAsFloat(TEXT("FlyingSpeed"));
	const float PitchAdjustment = BlackboardComp->GetValueAsFloat(TEXT("Pitch_Adjustment"));

	const FVector ForwardMovement = FVector(FlyingSpeed * DeltaSeconds, 0.f, 0.f);
	ControlledPawn->AddActorLocalOffset(ForwardMovement, true);

	const float YawDelta = CurrentYawSpeed * DeltaSeconds;
	const float PitchDelta = (CurrentPitchSpeed + PitchAdjustment) * DeltaSeconds;
	const float RollDelta = CurrentRollSpeed * DeltaSeconds;

	const FRotator DeltaRotation = FRotator(PitchDelta, YawDelta, RollDelta);
	ControlledPawn->AddActorLocalRotation(DeltaRotation, true);

	if (!AICon || AICon->FilteredGroundTargets.Num() == 0) return;
	AActor* Target = AICon->FilteredGroundTargets[0];
	if (!Target) return;

	const FVector TargetLocation = Target->GetActorLocation();
	const FVector SelfLocation = ControlledPawn->GetActorLocation();
	const FVector ForwardOffset = FVector(800.f, 0.f, 0.f);
	const FVector AdjustedLocation = FVector(TargetLocation.X, TargetLocation.Y, TargetLocation.Z + 800.f);

	const FVector Direction = (AdjustedLocation - SelfLocation).GetSafeNormal();
	const FRotator SelfRot = ControlledPawn->GetActorRotation();
	const FVector LocalDirection = SelfRot.UnrotateVector(Direction);
	
	const float InputX = LocalDirection.X;
	const float InputY = LocalDirection.Y;
	const float InputZ = LocalDirection.Z;

	const float FlyingTurnSpeed = BlackboardComp->GetValueAsFloat(TEXT("FlyingTurnSpeed"));

	const float TargetYawSpeed = InputY * FlyingTurnSpeed;
	CurrentYawSpeed = FMath::FInterpTo(CurrentYawSpeed, TargetYawSpeed, DeltaSeconds, 1.0f);

	
	const FRotator CurrentRotation = ControlledPawn->GetActorRotation();
	const float CurrentRoll = CurrentRotation.Roll;

	const float YawInput = CurrentYawSpeed * 0.5f;
	const float ReversedRoll = -CurrentRoll * 3.0f;

	const float InputYAbs = FMath::Abs(InputY);
	const bool bTurning = InputYAbs > 0.5f;

	const float TargetRollSpeed = bTurning ? YawInput : ReversedRoll;
	CurrentRollSpeed = FMath::FInterpTo(CurrentRollSpeed, TargetRollSpeed, DeltaSeconds, 1.0f);
	
	const float YawAbs = FMath::Abs(CurrentYawSpeed);

	const float DirectionalPitch = InputZ * FlyingTurnSpeed * 1.0f;
	const float SteeringPenalty = YawAbs * -0.2f;
	const float TargetPitchSpeed = DirectionalPitch + SteeringPenalty;

	CurrentPitchSpeed = FMath::FInterpTo(CurrentPitchSpeed, TargetPitchSpeed, DeltaSeconds, 2.0f);

}
