#include "BTService_FishCheckWater.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "FriendlyAI/AnimalFish.h"

UBTService_FishCheckWater::UBTService_FishCheckWater()
{
	NodeName = TEXT("Fish Check Water");
	Interval = 0.1f;
	RandomDeviation = 0.0f;

	IsInWaterKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_FishCheckWater, IsInWaterKey));
	LastValidWaterPositionKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_FishCheckWater, LastValidWaterPositionKey));
}

void UBTService_FishCheckWater::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return;
	}

	AAnimalFish* Fish = Cast<AAnimalFish>(AIController->GetPawn());
	if (!Fish)
	{
		return;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return;
	}

	bool bCurrentlyInWater = IsActorInWaterVolume(Fish);
	
	BlackboardComp->SetValueAsBool(TEXT("IsInWater"), bCurrentlyInWater);

	if (bCurrentlyInWater)
	{
		FVector CurrentPosition = Fish->GetActorLocation();
		BlackboardComp->SetValueAsVector(TEXT("LastValidWaterPosition"), CurrentPosition);
		Fish->SetLastValidWaterPosition(CurrentPosition);
	}
}

bool UBTService_FishCheckWater::IsActorInWaterVolume(AActor* Actor) const
{
	if (!Actor)
	{
		return false;
	}

	UWorld* World = Actor->GetWorld();
	if (!World)
	{
		return false;
	}

	TArray<AActor*> OverlappingActors;
	Actor->GetOverlappingActors(OverlappingActors, AActor::StaticClass());

	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (OverlappingActor && OverlappingActor->ActorHasTag(TEXT("WaterVolume")))
		{
			return true;
		}
	}

	return false;
}