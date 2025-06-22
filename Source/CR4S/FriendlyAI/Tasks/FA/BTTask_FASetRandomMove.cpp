#include "BTTask_FASetRandomMove.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_FASetRandomMove::UBTTask_FASetRandomMove()
{
	NodeName = TEXT("Set Random Move Point and Speed (Side Only)");
	bNotifyTick = true;
}

uint16 UBTTask_FASetRandomMove::GetInstanceMemorySize() const
{
	return sizeof(FBTTask_FASetRandomMoveMemory);
}

EBTNodeResult::Type UBTTask_FASetRandomMove::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FBTTask_FASetRandomMoveMemory* Memory = reinterpret_cast<FBTTask_FASetRandomMoveMemory*>(NodeMemory);

	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	APawn* ControlledPawn = AICon->GetPawn();
	if (!ControlledPawn) return EBTNodeResult::Failed;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSys) return EBTNodeResult::Failed;

	FVector Origin = ControlledPawn->GetActorLocation();
	
	const bool bLeft = FMath::RandBool();
	const FVector Right = ControlledPawn->GetActorRightVector();
	const FVector Direction = bLeft ? -Right : Right;
	
	const FVector DesiredPoint = Origin + Direction * 500.f;
	FNavLocation RandomPoint;

	if (!NavSys->ProjectPointToNavigation(DesiredPoint, RandomPoint, FVector(200.f)))
		return EBTNodeResult::Failed;

	BB->SetValueAsVector("RandomPointNearMyLocation", RandomPoint.Location);

	ACharacter* Character = Cast<ACharacter>(ControlledPawn);
	if (!Character) return EBTNodeResult::Failed;

	UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();
	if (!MoveComp) return EBTNodeResult::Failed;

	MoveComp->MaxAcceleration = 100.f;

	const float MinSpeed = BB->GetValueAsFloat("GroundSpeedMin");
	const float MaxSpeed = BB->GetValueAsFloat("GroundSpeedMax");
	const float RandomSpeed = FMath::FRandRange(MinSpeed, MaxSpeed);
	MoveComp->MaxWalkSpeed = RandomSpeed;
	
	Memory->TargetRotation = Direction.Rotation();
	return EBTNodeResult::InProgress;
}

void UBTTask_FASetRandomMove::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FBTTask_FASetRandomMoveMemory* Memory = reinterpret_cast<FBTTask_FASetRandomMoveMemory*>(NodeMemory);

	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return;

	APawn* ControlledPawn = AICon->GetPawn();
	if (!ControlledPawn) return;

	FRotator CurrentRotation = ControlledPawn->GetActorRotation();
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, Memory->TargetRotation, DeltaSeconds, Memory->RotationInterpSpeed);
	ControlledPawn->SetActorRotation(NewRotation);

	const float AngleDiff = FMath::Abs((NewRotation - Memory->TargetRotation).Yaw);
	if (AngleDiff < 1.f)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}