#include "BTService_FAFlyToPerchTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"
#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "FriendlyAI/AnimalFlying.h"
#include "Kismet/KismetMathLibrary.h"
#include "FriendlyAI/Controller/FAAIController.h"

UBTService_FAFlyToPerchTarget::UBTService_FAFlyToPerchTarget()
{
	NodeName = TEXT("Fly To Perch Target");
}

uint16 UBTService_FAFlyToPerchTarget::GetInstanceMemorySize() const
{
	return sizeof(FBTService_FAFlyToPerchTargetMemory);
}

void UBTService_FAFlyToPerchTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	FBTService_FAFlyToPerchTargetMemory* Memory = reinterpret_cast<FBTService_FAFlyToPerchTargetMemory*>(NodeMemory);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return;

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn) return;

	AAnimalFlying* FlyingAnimal = Cast<AAnimalFlying>(ControlledPawn);
	if (FlyingAnimal)
	{
		UCapsuleComponent* Capsule = FlyingAnimal->GetCapsuleComponent();
		if (Capsule)
		{
			Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			Capsule->SetCollisionProfileName(TEXT("Pawn"));
		}
	}

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;

	AFAAIController* FACon = Cast<AFAAIController>(AIController);
	if (!FACon || FACon->FilteredPerchTargets.Num() == 0) return;
	
	const float FlyingSpeed = BB->GetValueAsFloat("FlyingSpeed");
	const float PitchAdjustment = BB->GetValueAsFloat("Pitch_Adjustment");
	
	const FVector Offset = FVector(FlyingSpeed * DeltaSeconds, 0.f, 0.f);
	ControlledPawn->AddActorLocalOffset(Offset, true);
	
	const FVector From = ControlledPawn->GetActorLocation();
	FVector To = FACon->FilteredPerchTargets[0]->GetActorLocation();
	To.Z += 300.f;
	
	const FVector UnitDir = UKismetMathLibrary::GetDirectionUnitVector(From, To);
	const FVector Unrotated = ControlledPawn->GetActorRotation().UnrotateVector(UnitDir);
	
	const float DirY = Unrotated.Y;
	const float DirZ = Unrotated.Z;
	
	Memory->CurrentYawSpeed = FMath::FInterpTo(Memory->CurrentYawSpeed, FlyingSpeed * DirY, DeltaSeconds, 1.0f);
	
	const float A = Memory->CurrentYawSpeed * 0.5f;
	const float B = ControlledPawn->GetActorRotation().Roll * -3.0f;
	const bool bCond = FMath::Abs(DirY) > 0.5f;
	const float Selected = bCond ? B : A;
	Memory->CurrentRollSpeed = FMath::FInterpTo(Memory->CurrentRollSpeed, Selected, DeltaSeconds, 1.0f);

	const float PitchA = FlyingSpeed * DirZ * 1.0f;
	const float PitchB = FMath::Abs(Memory->CurrentYawSpeed) * -0.2f;
	Memory->CurrentPitchSpeed = FMath::FInterpTo(Memory->CurrentPitchSpeed, PitchA + PitchB, DeltaSeconds, 2.0f);
	
	const float Roll = Memory->CurrentRollSpeed * DeltaSeconds;
	const float Pitch = (PitchAdjustment + Memory->CurrentPitchSpeed) * DeltaSeconds;
	const float Yaw = Memory->CurrentYawSpeed * DeltaSeconds;
	
	const FRotator DeltaRot(Pitch, Yaw, Roll);
	ControlledPawn->AddActorLocalRotation(DeltaRot, true);
}