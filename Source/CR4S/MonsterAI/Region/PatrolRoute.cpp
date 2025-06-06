#include "MonsterAI/Region/PatrolRoute.h"
#include "Components/SplineComponent.h"

APatrolRoute::APatrolRoute()
{
	PrimaryActorTick.bCanEverTick = false;

	PatrolSpline = CreateDefaultSubobject<USplineComponent>(TEXT("PatrolSpline"));
	SetRootComponent(PatrolSpline);
}

void APatrolRoute::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

FVector APatrolRoute::GetPointAtIndex(int32 Index) const
{
	return PatrolSpline->GetLocationAtSplinePoint(Index, ESplineCoordinateSpace::World);
}

int32 APatrolRoute::GetNumberOfPoints() const
{
	return PatrolSpline->GetNumberOfSplinePoints();
}
