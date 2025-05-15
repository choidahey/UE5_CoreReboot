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
	GeometryCollectionComponent->DamageThreshold.Reset(2);
}

void AResourceGimmick::BeginPlay()
{
	Super::BeginPlay();

	GeometryCollectionComponent->SetVisibility(false);
	GeometryCollectionComponent->SetSimulatePhysics(false);
}

void AResourceGimmick::OnGimmickDestroy()
{
	UStaticMeshComponent* Mesh = GetStaticMeshComponent();
	if (IsValid(Mesh))
	{
		Mesh->SetVisibility(false);
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		GeometryCollectionComponent->SetVisibility(true);
		GeometryCollectionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GeometryCollectionComponent->SetSimulatePhysics(true);
	}

	Super::OnGimmickDestroy();
}


