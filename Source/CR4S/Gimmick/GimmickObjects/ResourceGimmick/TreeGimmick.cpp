#include "TreeGimmick.h"

#include "Gimmick/Components/DestructibleComponent.h"
#include "Gimmick/Manager/ItemGimmickSubsystem.h"

ATreeGimmick::ATreeGimmick()
	: bIsActorDestroyOnDestroyAction(true)
	  , DestroyDelay(0.f)
	  , StumpHealth(50.f)
	  , bIsTrunkDestroyed(false)
	  , ShakeDuration(0.5f)
	  , ShakeInterval(0.02f)
	  , ShakeIntensity(5.0f)
	  , OriginalLocation(FVector::ZeroVector)
	  , ElapsedTime(0.f)
{
	PrimaryActorTick.bCanEverTick = false;

	DestructibleComponent = CreateDefaultSubobject<UDestructibleComponent>(TEXT("DestructibleComponent"));

	TrunkMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrunkMeshComponent"));
	TrunkMeshComponent->SetupAttachment(RootComponent);

	bIsActorDestroyOnDestroyAction = false;
}

void ATreeGimmick::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(DestructibleComponent))
	{
		DestructibleComponent->OnTakeDamage.BindUObject(this, &ThisClass::OnGimmickTakeDamage);
		DestructibleComponent->OnDestroy.BindUObject(this, &ThisClass::OnGimmickDestroy);

		const UItemGimmickSubsystem* GimmickSubsystem = GetGameInstance()->GetSubsystem<UItemGimmickSubsystem>();
		if (IsValid(GimmickSubsystem))
		{
			// check(GimmickData);
			if (const FBaseGimmickData* GimmickData = GimmickSubsystem->FindGimmickData(GetGimmickDataRowName()))
			{
				DestructibleComponent->SetMaxHealth(GimmickData->GimmickMaxHealth);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("%s is not found in GimmickData"), *GetGimmickDataRowName().ToString());
			}
		}
	}
	
	OriginalLocation = GetActorLocation();
}

void ATreeGimmick::OnGimmickTakeDamage(const float DamageAmount, const float CurrentHealth)
{
	UE_LOG(LogTemp, Warning, TEXT("Gimmick is damaged / DamageAmount: %.1f / CurrentHealth: %.1f"), DamageAmount,
		   CurrentHealth);
	
	StartShake();
}

void ATreeGimmick::OnGimmickDestroy()
{
	/** BEFORE THE TRUNK IS DESTROYED */
	if (!bIsTrunkDestroyed)
	{
		bIsTrunkDestroyed = true;

		TrunkMeshComponent->SetVisibility(false);
		TrunkMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		bIsActorDestroyOnDestroyAction = true;
		DestructibleComponent->SetMaxHealth(StumpHealth);
	}
	/** AFTER THE TRUNK IS DESTROYED */
	else
	{
		if (!bIsActorDestroyOnDestroyAction)
		{
			return;
		}

		if (DestroyDelay == 0.f)
		{
			DelayedDestroy();
		}

		GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &ThisClass::DelayedDestroy, DestroyDelay, false);
	}
}

void ATreeGimmick::DelayedDestroy()
{
	UE_LOG(LogTemp, Warning, TEXT("Gimmick is destroyed"));

	Destroy();
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
