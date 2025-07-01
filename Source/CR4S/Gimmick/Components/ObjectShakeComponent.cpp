#include "ObjectShakeComponent.h"

UObjectShakeComponent::UObjectShakeComponent()
	: bCanShake(true),
	  ShakeDuration(0.5f),
	  ShakeInterval(0.02f),
	  ShakeIntensity(2.5f),
	  OriginalLocation(FVector::ZeroVector),
	  ElapsedTime(0.f),
	  LastOffset(FVector::ZeroVector)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UObjectShakeComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerActor = GetOwner();
	if (IsValid(OwnerActor))
	{
		OriginalLocation = OwnerActor->GetActorLocation();
	}
}

void UObjectShakeComponent::Shake()
{
	if (bCanShake)
	{
		StartShake();
	}
}

void UObjectShakeComponent::StartShake()
{
	GetWorld()->GetTimerManager().SetTimer(
		ShakeTimerHandle,
		this,
		&ThisClass::PerformShake,
		ShakeInterval,
		true
	);
}

void UObjectShakeComponent::PerformShake()
{
	ElapsedTime += ShakeInterval;

	if (ElapsedTime >= ShakeDuration || !IsValid(OwnerActor))
	{
		StopShake();
		return;
	}

	OwnerActor->AddActorWorldOffset(-LastOffset, false, nullptr, ETeleportType::TeleportPhysics);

	const FVector NewOffset = FMath::VRand() * ShakeIntensity;
	LastOffset = NewOffset;

	OwnerActor->AddActorWorldOffset(LastOffset, false, nullptr, ETeleportType::TeleportPhysics);
}

void UObjectShakeComponent::StopShake()
{
	GetWorld()->GetTimerManager().ClearTimer(ShakeTimerHandle);
	
	if (IsValid(OwnerActor))
	{
		OwnerActor->AddActorWorldOffset(-LastOffset, false, nullptr, ETeleportType::TeleportPhysics);
	}
	
	ElapsedTime = 0.f;
	LastOffset = FVector::ZeroVector;
}
