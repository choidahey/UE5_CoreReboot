#include "MonsterAI/Animations/AnimNotifyState/AnimNotifyState_MoveToCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "CR4S.h"

void UAnimNotifyState_MoveToCharacter::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (!MeshComp) return;

	CachedCharacter = Cast<ACharacter>(MeshComp->GetOwner());
	if (!CachedCharacter.IsValid()) return;

	CachedController = Cast<AAIController>(CachedCharacter->GetController());
	if (!CachedController.IsValid()) return;

	TargetActor = GetTargetActor();
	ElapsedMoveTime = 0.f;

	if (UCharacterMovementComponent* MoveComp = CachedCharacter->GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = MoveSpeed;
	}

	StartMoveTo();
}

void UAnimNotifyState_MoveToCharacter::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	ElapsedMoveTime += FrameDeltaTime;
	if (ElapsedMoveTime >= MoveInterval)
	{
		StartMoveTo();
		ElapsedMoveTime = 0.f;
	}
}

void UAnimNotifyState_MoveToCharacter::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	if (CachedController.IsValid())
	{
		CachedController->StopMovement();
	}

	if (bJumpOnEnd)
	{
		LaunchJumpTowardTarget();
	}
}

void UAnimNotifyState_MoveToCharacter::StartMoveTo()
{
	if (!CachedController.IsValid() || !CachedCharacter.IsValid() || !TargetActor) return;

	const FVector RawTargetLocation = TargetActor->GetActorLocation();
	FVector GoalLocation = RawTargetLocation;

	if (UWorld* World = CachedCharacter->GetWorld())
	{
		if (UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(World))
		{
			FNavLocation ProjectedLocation;
			FVector ProjectExtent = FVector(500, 500, 5000);
			if (NavSys->ProjectPointToNavigation(RawTargetLocation, ProjectedLocation, ProjectExtent))
			{
				GoalLocation = ProjectedLocation.Location;
			}
		}
	}

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(GoalLocation);
	MoveRequest.SetAcceptanceRadius(AcceptableRadius);
	MoveRequest.SetUsePathfinding(true);
	MoveRequest.SetCanStrafe(true);
	MoveRequest.SetReachTestIncludesAgentRadius(false);

	FPathFollowingRequestResult Result = CachedController->MoveTo(MoveRequest);

#if WITH_EDITOR
	UE_LOG(LogMonster, Log, TEXT("[MoveToCharacter - BEFORE] MoveTo Result: Code=%d (%s), GoalLocation: %s"),
		static_cast<int32>(Result.Code),
		Result.Code == EPathFollowingRequestResult::RequestSuccessful ? TEXT("RequestSuccessful") :
		Result.Code == EPathFollowingRequestResult::AlreadyAtGoal ? TEXT("AlreadyAtGoal") :
		Result.Code == EPathFollowingRequestResult::Failed ? TEXT("Failed") :
		TEXT("Unknown"),
		*GoalLocation.ToString()
	);
#endif

}

void UAnimNotifyState_MoveToCharacter::LaunchJumpTowardTarget()
{
	if (!CachedCharacter.IsValid()) return;

	const FVector CurrentLocation = CachedCharacter->GetActorLocation();
	const FVector TargetLocation = (bUseTargetActor && TargetActor) ? TargetActor->GetActorLocation() : MoveToLocation;

	FVector FlatDir = TargetLocation - CurrentLocation;
	FlatDir.Z = 0.f;

	if (FlatDir.Size() > MaxJumpDistance)
	{
		FlatDir = FlatDir.GetSafeNormal() * MaxJumpDistance;
	}

	FVector LaunchVelocity = FlatDir;
	LaunchVelocity.Z = JumpZImpulse;

	CachedCharacter->LaunchCharacter(LaunchVelocity, true, true);
}

AActor* UAnimNotifyState_MoveToCharacter::GetTargetActor() const
{
	if (!CachedController.IsValid()) return nullptr;

	if (UBlackboardComponent* BB = CachedController->GetBlackboardComponent())
	{
		return Cast<AActor>(BB->GetValueAsObject(TargetBlackboardKey));
	}
	return nullptr;
}
