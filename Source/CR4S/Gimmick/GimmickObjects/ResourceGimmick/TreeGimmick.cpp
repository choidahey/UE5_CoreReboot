#include "TreeGimmick.h"

#include "Gimmick/Components/DestructibleComponent.h"

ATreeGimmick::ATreeGimmick()
	: bIsTrunkDestroyed(false),
	  StumpHealth(50.f),
	  ImpulseStrength(100.f),
	  RemoveTrunkDelay(5.f)
{
	PrimaryActorTick.bCanEverTick = false;

	TrunkMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrunkMeshComponent"));
	TrunkMeshComponent->SetupAttachment(RootComponent);

	DestructibleComponent->SetCanRepair(false);
}

void ATreeGimmick::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(TrunkMeshComponent))
	{
		OriginTrunkTransform = TrunkMeshComponent->GetRelativeTransform();
	}
}

void ATreeGimmick::OnGimmickDestroy(AActor* DamageCauser)
{
	// Before The Trunk Is Destroyed
	if (!bIsTrunkDestroyed)
	{
		GetResources(DamageCauser);
		HandleDestroyTrunk(DamageCauser);
	}
	// After The Trunk Is Destroyed
	else
	{
		GetWorldTimerManager().ClearTimer(RemoveTrunkTimerHandle);
		Super::OnGimmickDestroy(DamageCauser);
	}
}

void ATreeGimmick::HandleDestroyTrunk(const AActor* DamageCauser)
{
	bIsTrunkDestroyed = true;

	if (IsValid(DestructibleComponent))
	{
		DestructibleComponent->SetMaxHealth(StumpHealth);
	}
	
	if (!IsValid(TrunkMeshComponent))
	{
		return;
	}
	
	TrunkMeshComponent->SetSimulatePhysics(true);

	FVector Direction = GetActorLocation() - DamageCauser->GetActorLocation();
	Direction.Z = 0.f;
	Direction.Normalize();

	const float Strength = TrunkMeshComponent->GetMass() * ImpulseStrength;

	TrunkMeshComponent->AddImpulse(Direction * Strength);

	GetWorldTimerManager().SetTimer(
		RemoveTrunkTimerHandle,
		this,
		&ThisClass::RemoveTrunk,
		RemoveTrunkDelay,
		false
	);
}

void ATreeGimmick::RemoveTrunk() const
{
	TrunkMeshComponent->SetSimulatePhysics(false);
	TrunkMeshComponent->SetVisibility(false);
	TrunkMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TrunkMeshComponent->SetRelativeTransform(OriginTrunkTransform);
}
