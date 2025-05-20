#include "TreeGimmick.h"

#include "Gimmick/Components/DestructibleComponent.h"

ATreeGimmick::ATreeGimmick()
	: StumpHealth(50.f)
	  , bIsTrunkDestroyed(false)
	  , ShakeDuration(0.5f)
	  , ShakeInterval(0.02f)
	  , ShakeIntensity(5.0f)
	  , OriginalLocation(FVector::ZeroVector)
	  , ElapsedTime(0.f)
{
	PrimaryActorTick.bCanEverTick = false;

	TrunkMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrunkMeshComponent"));
	TrunkMeshComponent->SetupAttachment(RootComponent);
	
	bIsActorDestroyOnDestroyAction = false;
}

void ATreeGimmick::BeginPlay()
{
	Super::BeginPlay();

	OriginalLocation = GetActorLocation();
}

void ATreeGimmick::OnGimmickTakeDamage(const float DamageAmount, const float CurrentHealth)
{
	Super::OnGimmickTakeDamage(DamageAmount, CurrentHealth);

	StartShake();
}

void ATreeGimmick::OnGimmickDestroy()
{
	Super::OnGimmickDestroy();

	if (!bIsTrunkDestroyed)
	{
		bIsTrunkDestroyed = true;

		TrunkMeshComponent->SetVisibility(false);
		TrunkMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		bIsActorDestroyOnDestroyAction = true;
		DestructibleComponent->SetMaxHealth(StumpHealth);
	}
}

void ATreeGimmick::StartShake()
{
	UE_LOG(LogTemp, Warning, TEXT("==== ShakeStart ===="));
	GetWorldTimerManager().SetTimer(
		ShakeTimerHandle,
		this,
		&ThisClass::PerformShake,
		ShakeInterval,
		true
	);
}

void ATreeGimmick::PerformShake()
{
	ElapsedTime += ShakeInterval;

	if (ElapsedTime >= ShakeDuration)
	{
		StopShake();
		return;
	}

	const FVector RandomOffset = FMath::VRand() * ShakeIntensity;

	SetActorLocation(OriginalLocation + RandomOffset, false, nullptr, ETeleportType::TeleportPhysics);
}

void ATreeGimmick::StopShake()
{
	UE_LOG(LogTemp, Warning, TEXT("==== ShakeEnd ===="));
	ElapsedTime = 0.f;
	GetWorldTimerManager().ClearTimer(ShakeTimerHandle);

	SetActorLocation(OriginalLocation, false, nullptr, ETeleportType::TeleportPhysics);
}
