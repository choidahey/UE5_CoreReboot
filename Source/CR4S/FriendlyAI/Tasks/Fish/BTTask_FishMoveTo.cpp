#include "BTTask_FishMoveTo.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FriendlyAI/AnimalFish.h"

UBTTask_FishMoveTo::UBTTask_FishMoveTo()
{
	NodeName = TEXT("Fish Move To");
	bNotifyTick = true;
	bNotifyTaskFinished = true;

	PatrolTargetKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FishMoveTo, PatrolTargetKey));
}

EBTNodeResult::Type UBTTask_FishMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	AAnimalFish* Fish = Cast<AAnimalFish>(AIController->GetPawn());
	if (!Fish)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	FVector TargetLocation = BlackboardComp->GetValueAsVector(TEXT("PatrolTarget"));
	if (TargetLocation == FVector::ZeroVector)
	{
		return EBTNodeResult::Failed;
	}
	
	FFishMoveToMemory* TaskMemory = reinterpret_cast<FFishMoveToMemory*>(NodeMemory);
	TaskMemory->TargetLocation = TargetLocation;
	TaskMemory->StartTime = GetWorld()->GetTimeSeconds();
	TaskMemory->NextWiggleTime = TaskMemory->StartTime + 0.5f;
	TaskMemory->WiggleOffset = FVector::ZeroVector;

	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTask_FishMoveTo::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController)
	{
		AIController->StopMovement();
	}

	return EBTNodeResult::Aborted;
}

void UBTTask_FishMoveTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	AAnimalFish* Fish = Cast<AAnimalFish>(AIController->GetPawn());
	if (!Fish)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	FFishMoveToMemory* TaskMemory = reinterpret_cast<FFishMoveToMemory*>(NodeMemory);
	float CurrentTime = GetWorld()->GetTimeSeconds();
	
	if (CurrentTime - TaskMemory->StartTime > 5.0f)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	FVector CurrentLocation = Fish->GetActorLocation();
	FVector TargetLocation = TaskMemory->TargetLocation;
	
	if (HasReachedTarget(CurrentLocation, TargetLocation))
	{
		AIController->StopMovement();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
	
	FVector WiggleMovement = GetWiggleMovement(CurrentTime, TaskMemory);
	FVector AdjustedTarget = TargetLocation + WiggleMovement;
	
	float DistanceToTarget = FVector::Dist(CurrentLocation, TargetLocation);
	UCharacterMovementComponent* MovementComp = Fish->GetCharacterMovement();
	
	if (MovementComp)
	{
		float BaseSpeed = MovementComp->MaxWalkSpeed;
		float AdjustedSpeed = BaseSpeed;
		
		if (DistanceToTarget < 150.0f)
		{
			float SpeedMultiplier = FMath::Max(0.3f, DistanceToTarget / 150.0f);
			AdjustedSpeed = BaseSpeed * SpeedMultiplier;
		}
		
		FVector Direction = (AdjustedTarget - CurrentLocation).GetSafeNormal();
		FVector Velocity = Direction * AdjustedSpeed;

		MovementComp->Velocity = FMath::VInterpTo(MovementComp->Velocity, Velocity, DeltaSeconds, 2.0f);
		
		if (!Direction.IsZero())
		{
			FRotator TargetRotation = Direction.Rotation();
			FRotator CurrentRotation = Fish->GetActorRotation();
			
			TargetRotation.Pitch = FMath::Clamp(TargetRotation.Pitch, -45.0f, 45.0f);
			
			FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, 3.0f);
			Fish->SetActorRotation(NewRotation);
		}
	}
}

bool UBTTask_FishMoveTo::HasReachedTarget(const FVector& CurrentLocation, const FVector& TargetLocation) const
{
	return FVector::Dist(CurrentLocation, TargetLocation) <= 50.0f;
}

FVector UBTTask_FishMoveTo::GetWiggleMovement(float CurrentTime, FFishMoveToMemory* TaskMemory) const
{
	if (CurrentTime >= TaskMemory->NextWiggleTime)
	{
		TaskMemory->WiggleOffset = FVector(
			FMath::RandRange(-10.0f, 10.0f),
			FMath::RandRange(-10.0f, 10.0f),
			FMath::RandRange(-10.0f, 10.0f)
		);
		TaskMemory->NextWiggleTime = CurrentTime + 0.5f;
	}

	return TaskMemory->WiggleOffset;
}

uint16 UBTTask_FishMoveTo::GetInstanceMemorySize() const
{
	return sizeof(FFishMoveToMemory);
}