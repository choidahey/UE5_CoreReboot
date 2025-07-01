#include "BTTask_FishPatrolLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "Components/BoxComponent.h"
#include "FriendlyAI/AnimalFish.h"
#include "Kismet/GameplayStatics.h"

UBTTask_FishPatrolLocation::UBTTask_FishPatrolLocation()
{
	NodeName = TEXT("Fish Patrol Location");
	bNotifyTick = false;

	PatrolTargetKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FishPatrolLocation, PatrolTargetKey));
}

EBTNodeResult::Type UBTTask_FishPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	FVector PatrolLocation = FindPatrolLocation(Fish);
	
	if (PatrolLocation != FVector::ZeroVector)
	{
		BlackboardComp->SetValueAsVector(TEXT("PatrolTarget"), PatrolLocation);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

FVector UBTTask_FishPatrolLocation::FindPatrolLocation(AActor* Fish) const
{
	if (!Fish)
	{
		return FVector::ZeroVector;
	}

	UBoxComponent* WaterBox = FindWaterVolumeBox(Fish);
	if (!WaterBox)
	{
		return FVector::ZeroVector;
	}

	FVector CurrentLocation = Fish->GetActorLocation();
	FVector ForwardVector = Fish->GetActorForwardVector();

	return GetRandomLocationInBox(WaterBox, CurrentLocation, ForwardVector);
}

UBoxComponent* UBTTask_FishPatrolLocation::FindWaterVolumeBox(AActor* Fish) const
{
	if (!Fish)
	{
		return nullptr;
	}

	UWorld* World = Fish->GetWorld();
	if (!World)
	{
		return nullptr;
	}
	
	TArray<AActor*> OverlappingActors;
	Fish->GetOverlappingActors(OverlappingActors, AActor::StaticClass());

	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (OverlappingActor && OverlappingActor->ActorHasTag(TEXT("WaterVolume")))
		{
			if (UBoxComponent* BoxComp = OverlappingActor->FindComponentByClass<UBoxComponent>())
			{
				return BoxComp;
			}
		}
	}
	
	TArray<AActor*> AllWaterVolumes;
	UGameplayStatics::GetAllActorsWithTag(World, TEXT("WaterVolume"), AllWaterVolumes);

	AActor* ClosestWaterVolume = nullptr;
	float ClosestDistance = FLT_MAX;

	for (AActor* WaterVolume : AllWaterVolumes)
	{
		if (WaterVolume)
		{
			float Distance = FVector::Dist(Fish->GetActorLocation(), WaterVolume->GetActorLocation());
			if (Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				ClosestWaterVolume = WaterVolume;
			}
		}
	}

	if (ClosestWaterVolume)
	{
		return ClosestWaterVolume->FindComponentByClass<UBoxComponent>();
	}

	return nullptr;
}

FVector UBTTask_FishPatrolLocation::GetRandomLocationInBox(UBoxComponent* BoxComp, const FVector& CurrentLocation, const FVector& ForwardVector) const
{
	if (!BoxComp)
	{
		return FVector::ZeroVector;
	}

	FVector BoxExtent = BoxComp->GetScaledBoxExtent();
	FVector BoxCenter = BoxComp->GetComponentLocation();
	
	FVector BoxMin = BoxCenter - BoxExtent;
	FVector BoxMax = BoxCenter + BoxExtent;
	
	float PatrolRadius = 300.0f;
	FVector PatrolMin = FVector::Max(BoxMin, CurrentLocation - FVector(PatrolRadius));
	FVector PatrolMax = FVector::Min(BoxMax, CurrentLocation + FVector(PatrolRadius));
	
	FVector BestLocation = FVector::ZeroVector;
	int32 MaxAttempts = 10;

	for (int32 i = 0; i < MaxAttempts; ++i)
	{
		FVector RandomLocation = FVector(
			FMath::RandRange(PatrolMin.X, PatrolMax.X),
			FMath::RandRange(PatrolMin.Y, PatrolMax.Y),
			FMath::RandRange(PatrolMin.Z, PatrolMax.Z)
		);

		FVector ToTarget = (RandomLocation - CurrentLocation).GetSafeNormal();
		float DotProduct = FVector::DotProduct(ForwardVector, ToTarget);
		
		if (DotProduct > 0.0f)
		{
			return RandomLocation;
		}
		
		if (BestLocation == FVector::ZeroVector)
		{
			BestLocation = RandomLocation;
		}
	}
	
	return BestLocation != FVector::ZeroVector ? BestLocation : FVector(
		FMath::RandRange(PatrolMin.X, PatrolMax.X),
		FMath::RandRange(PatrolMin.Y, PatrolMax.Y),
		FMath::RandRange(PatrolMin.Z, PatrolMax.Z)
	);
}