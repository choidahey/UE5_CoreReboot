#include "BaseDestructObject.h"

#include "GeometryCollection/GeometryCollectionComponent.h"

ABaseDestructObject::ABaseDestructObject()
	: DestroyImpulseRadius(50.f),
	  DestroyImpulseStrength(50.f),
	  LifeTime(2.f)

{
	PrimaryActorTick.bCanEverTick = false;
	
	GeometryCollectionComponent
		= CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollectionComponent"));
	SetRootComponent(GeometryCollectionComponent);
	
	GeometryCollectionComponent->DamageThreshold.Init(0.f, 1);
	GeometryCollectionComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	GeometryCollectionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GeometryCollectionComponent->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);

	AActor::SetLifeSpan(LifeTime);
}

void ABaseDestructObject::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(GeometryCollectionComponent))
	{
		GeometryCollectionComponent->AddRadialImpulse(
			GeometryCollectionComponent->GetComponentLocation(),
			DestroyImpulseRadius,
			DestroyImpulseStrength,
			RIF_Linear,
			true
		);
	}
}
