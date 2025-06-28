#include "ObjectShakeComponent.h"

UObjectShakeComponent::UObjectShakeComponent()
	: bCanShake(true),
	  ShakeDuration(0.5f),
	  ShakeInterval(0.02f),
	  ShakeIntensity(2.5f),
	  OriginalLocation(FVector::ZeroVector),
	  ElapsedTime(0.f)
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

	const FVector RandomOffset = FMath::VRand() * ShakeIntensity;

	OwnerActor->SetActorLocation(OriginalLocation + RandomOffset, false, nullptr, ETeleportType::TeleportPhysics);
}

void UObjectShakeComponent::StopShake()
{
	ElapsedTime = 0.f;
	GetWorld()->GetTimerManager().ClearTimer(ShakeTimerHandle);

	OwnerActor->SetActorLocation(OriginalLocation, false, nullptr, ETeleportType::TeleportPhysics);
}

