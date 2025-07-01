#include "BTService_FAFlySkyTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "AIController.h"

UBTService_FAFlySkyTarget::UBTService_FAFlySkyTarget()
{
	NodeName = TEXT("Fly To Sky Target");
}

uint16 UBTService_FAFlySkyTarget::GetInstanceMemorySize() const
{
	return sizeof(FBTService_FAFlySkyTargetMemory);
}

void UBTService_FAFlySkyTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	FBTService_FAFlySkyTargetMemory* Memory = reinterpret_cast<FBTService_FAFlySkyTargetMemory*>(NodeMemory);

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
	Memory->CurrentYawSpeed = FMath::FInterpTo(Memory->CurrentYawSpeed, TargetYawSpeed, DeltaSeconds, 1.0f);

	float PitchTarget = OffsetZ * TurnSpeed;
	float YawInfluence = -0.2f * FMath::Abs(Memory->CurrentYawSpeed);
	float TargetPitchSpeed = PitchTarget + YawInfluence + PitchAdjustment;
	Memory->CurrentPitchSpeed = FMath::FInterpTo(Memory->CurrentPitchSpeed, TargetPitchSpeed, DeltaSeconds, 1.0f);

	float RollOptionA = -3.0f * ActorRotation.Roll;
	float RollOptionB = 0.5f * Memory->CurrentYawSpeed;
	float SelectedRoll = (FMath::Abs(OffsetY) > 0.5f) ? RollOptionA : RollOptionB;
	Memory->CurrentRollSpeed = FMath::FInterpTo(Memory->CurrentRollSpeed, SelectedRoll, DeltaSeconds, 1.0f);

	FRotator FinalRotation = FRotator(
		Memory->CurrentPitchSpeed * DeltaSeconds,
		Memory->CurrentYawSpeed * DeltaSeconds,
		Memory->CurrentRollSpeed * DeltaSeconds
	);
	ControlledPawn->AddActorLocalRotation(FinalRotation);
}