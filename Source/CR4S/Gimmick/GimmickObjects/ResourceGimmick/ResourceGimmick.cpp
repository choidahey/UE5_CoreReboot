#include "ResourceGimmick.h"

#include "GeometryCollection/GeometryCollectionComponent.h"

AResourceGimmick::AResourceGimmick()
{
	PrimaryActorTick.bCanEverTick = false;

	GeometryCollectionComponent = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollectionComponent"));
	GeometryCollectionComponent->SetupAttachment(RootComponent);
	GeometryCollectionComponent->SetVisibility(false);
	GeometryCollectionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GeometryCollectionComponent->SetSimulatePhysics(false);
	GeometryCollectionComponent->DamageThreshold.Init(0.f, 2);
}

void AResourceGimmick::BeginPlay()
{
	Super::BeginPlay();

	GeometryCollectionComponent->SetVisibility(false);
	GeometryCollectionComponent->SetSimulatePhysics(false);
}

void AResourceGimmick::OnGimmickDestroy()
{
	if (IsValid(GimmickMeshComponent))
	{
		GimmickMeshComponent->SetVisibility(false);
		GimmickMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		GeometryCollectionComponent->SetVisibility(true);
		GeometryCollectionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GeometryCollectionComponent->SetSimulatePhysics(true);
	}

	Super::OnGimmickDestroy();
}


