#include "BTService_GeneratePathPoints.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "AIController.h"
#include "GameFramework/Actor.h"
//#include "Engine/TargetPoint.h"
#include "DrawDebugHelpers.h"

UBTService_GeneratePathPoints::UBTService_GeneratePathPoints()
{
	NodeName = TEXT("Generate Path Points");
	Interval = 1.0f;
	RandomDeviation = 0.0f;

	PlayerKey.SelectedKeyName = "Player";
	TargetPointKey.SelectedKeyName = "TargetPoint";
}

void UBTService_GeneratePathPoints::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;

	AActor* PlayerActor = Cast<AActor>(BB->GetValueAsObject(PlayerKey.SelectedKeyName));
	APawn* ControlledPawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;

	if (!PlayerActor || !ControlledPawn) return;

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSys) return;

	UNavigationPath* Path = NavSys->FindPathToActorSynchronously(GetWorld(), ControlledPawn->GetActorLocation(), PlayerActor);
	if (!Path || Path->PathPoints.Num() < 2) return;

	const TArray<FVector>& FullPoints = Path->PathPoints;
	const int32 TotalPoints = FullPoints.Num();
	
	TArray<FVector> SampledPoints;
	const int32 DesiredCount = FMath::Clamp(NumPoints, 2, TotalPoints);
	const float IntervalRatio = 1.f / (DesiredCount - 1);

	for (int32 i = 0; i < DesiredCount; ++i)
	{
		float Alpha = IntervalRatio * i;
		int32 IndexF = FMath::FloorToInt(Alpha * (TotalPoints - 1));
		int32 IndexC = FMath::CeilToInt(Alpha * (TotalPoints - 1));
		float LerpAlpha = (Alpha * (TotalPoints - 1)) - IndexF;

		FVector Point = FMath::Lerp(FullPoints[IndexF], FullPoints[IndexC], LerpAlpha);
		SampledPoints.Add(Point);
	}
	
	TArray<FNavLocation> ProjectedPoints;
	FNavLocation Projected;
	for (int32 i = 0; i < SampledPoints.Num(); ++i)
	{
		if (NavSys->ProjectPointToNavigation(SampledPoints[i], Projected))
		{
			FColor DebugColor = (i == SampledPoints.Num() - 1) ? FColor::Green : FColor::Yellow;
			float Radius = (i == SampledPoints.Num() - 1) ? 50.f : 20.f;

			DrawDebugSphere(GetWorld(), Projected.Location, Radius, 12, DebugColor, false, 1.0f);
			ProjectedPoints.Add(Projected);
		}
	}
	
	float MinDistSq = TNumericLimits<float>::Max();
	int32 ClosestIndex = 0;
	for (int32 j = 0; j < ProjectedPoints.Num(); ++j)
	{
		float DistSq = FVector::DistSquared(ProjectedPoints[j].Location, PlayerActor->GetActorLocation());
		if (DistSq < MinDistSq)
		{
			MinDistSq = DistSq;
			ClosestIndex = j;
		}
	}

	const FVector ClosestLocation = ProjectedPoints[ClosestIndex].Location;
	
	AActor* Target = nullptr;
	const bool bPathBlocked = Path->IsPartial();

	TArray<FHitResult> Hits;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(ControlledPawn);
	Params.AddIgnoredActor(PlayerActor);

	bool bBlocked = GetWorld()->LineTraceMultiByChannel(
		Hits,
		ControlledPawn->GetActorLocation(),
		PlayerActor->GetActorLocation(),
		ECC_Visibility,
		Params
	);

	if (bPathBlocked || bBlocked)
	{
		for (const FHitResult& Hit : Hits)
		{
			if (Hit.GetActor() && Hit.GetActor()->ActorHasTag("House"))
			{
				Target = Hit.GetActor();
				break;
			}
		}
	}

	if (Target)
	{
		BB->SetValueAsObject(TargetPointKey.SelectedKeyName, Target);
		BB->SetValueAsObject(FName("TargetActor"), Target);
		BB->SetValueAsBool(FName("IsObstacleBlocking"), true);
	}
	else
	{
		BB->SetValueAsVector(TargetPointKey.SelectedKeyName, ClosestLocation);
		BB->SetValueAsBool(FName("IsObstacleBlocking"), false);
	}
}
