#include "BTService_FAObstacleAvoid.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "FriendlyAI/Controller/FAAIController.h"
#include "FriendlyAI/AnimalFlying.h"

UBTService_FAObstacleAvoid::UBTService_FAObstacleAvoid()
{
	NodeName = TEXT("Obstacle Avoid");
}

void UBTService_FAObstacleAvoid::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return;

	APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControlledPawn) return;

	AAnimalFlying* Animal = Cast<AAnimalFlying>(ControlledPawn);
	if (!Animal) return;

	if (!BlackboardComp->GetValueAsBool("DoADownTrace")) return;

	const float DownTraceDistance = BlackboardComp->GetValueAsFloat("DownTraceDistance");
	const FVector Start = ControlledPawn->GetActorLocation() - FVector(0, 0, 120);

	const FRotator Rotation = ControlledPawn->GetActorRotation();
	const FVector Forward = Rotation.Vector() * DownTraceDistance;
	const FVector Down = Rotation.RotateVector(FVector(0, 0, -1)) * DownTraceDistance;

	const FVector End = Start + Forward + Down;

	FHitResult HitResult;
	FCollisionQueryParams TraceParams(FName(TEXT("GroundTrace")), true, ControlledPawn);
	TraceParams.bTraceComplex = true;

	bool bHit = ControlledPawn->GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, TraceParams);

	if (bHit)
	{
		const float Distance = FVector::Dist(ControlledPawn->GetActorLocation(), HitResult.Location);
		BlackboardComp->SetValueAsBool("FlapFast", true);
		BlackboardComp->SetValueAsFloat("DistanceToTheGround", Distance);

		if (BlackboardComp->GetValueAsBool("AreWeFlying"))
		{
			UCharacterMovementComponent* MoveComp = Animal->GetCharacterMovement();
			if (MoveComp)
			{
				const FVector UpVector = ControlledPawn->GetActorRotation().RotateVector(FVector(0.f, 0.f, 1.f));
				const FVector Thrust = UpVector * BlackboardComp->GetValueAsFloat("VerticalThrustAmount");
				MoveComp->Velocity = MoveComp->Velocity + Thrust;
			}
			BlackboardComp->SetValueAsFloat("Pitch_Adjustment", 80.0f);
		}
	}
	else
	{
		BlackboardComp->SetValueAsBool("FlapFast", false);
		BlackboardComp->SetValueAsFloat("DistanceToTheGround", DownTraceDistance);

		if (BlackboardComp->GetValueAsBool("AreWeFlying"))
		{
			FTimerHandle TimerHandle;
			ControlledPawn->GetWorldTimerManager().SetTimer(TimerHandle, [BlackboardComp]()
			{
				if (BlackboardComp)
				{
					BlackboardComp->SetValueAsFloat("Pitch_Adjustment", 0.0f);
				}
			}, 2.0f, false);
		}
	}
}
