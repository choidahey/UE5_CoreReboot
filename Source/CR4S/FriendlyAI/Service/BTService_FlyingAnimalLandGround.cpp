#include "BTService_FlyingAnimalLandGround.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "FriendlyAI/AnimalFlying.h"
#include "FriendlyAI/Component/FlyingMovementComponent.h"

UBTService_FlyingAnimalLandGround::UBTService_FlyingAnimalLandGround()
{
	NodeName = TEXT("Land When Close To Ground Target");
}

void UBTService_FlyingAnimalLandGround::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControlledPawn) return;

	AAnimalFlying* FlyingAnimal = Cast<AAnimalFlying>(ControlledPawn);
	if (!FlyingAnimal) return;

	UFlyingMovementComponent* FlyMove = FlyingAnimal->FlyingComp;
	if (!FlyMove) return;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;

	AActor* GroundTarget = Cast<AActor>(BB->GetValueAsObject(TEXT("BirdGroundTarget")));
	if (!GroundTarget) return;

	float Distance = ControlledPawn->GetDistanceTo(GroundTarget);
	if (Distance <= LandingDistanceThreshold)
	{
		BB->SetValueAsBool(TEXT("AreWeFlying"), false);
		BB->SetValueAsBool(TEXT("AreWePerched"), false);
		BB->SetValueAsBool(TEXT("TimeToLand"), false);
		BB->SetValueAsBool(TEXT("AreWeWalking"), true);
	}
}
