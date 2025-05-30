#include "TreeGimmick.h"

#include "CR4S.h"
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

	OriginalLocation = GetActorLocation();

	if (CR4S_VALIDATE(LogGimmick, IsValid(DestructibleComponent)))
	{
		DestructibleComponent->OnTakeDamage.BindDynamic(this, &ThisClass::OnGimmickTakeDamage);
		DestructibleComponent->OnDestroy.BindDynamic(this, &ThisClass::OnGimmickDestroy);

		const UItemGimmickSubsystem* GimmickSubsystem = GetGameInstance()->GetSubsystem<UItemGimmickSubsystem>();

		if (!CR4S_VALIDATE(LogGimmick, IsValid(GimmickSubsystem)))
		{
			return;
		}

		if (const FBaseGimmickData* GimmickData = GimmickSubsystem->FindGimmickData(GetGimmickDataRowName()))
		{
			DestructibleComponent->SetMaxHealth(GimmickData->GimmickMaxHealth);
		}
	}
}

void ATreeGimmick::OnGimmickTakeDamage(AActor* DamageCauser, const float DamageAmount, const float CurrentHealth)
{
	CR4S_Log(LogGimmick, Warning, TEXT("Gimmick is damaged / DamageAmount: %.1f / CurrentHealth: %.1f"),
	         DamageAmount, CurrentHealth);

	StartShake();
}

void ATreeGimmick::OnGimmickDestroy(AActor* DamageCauser)
{
	GetResources(DamageCauser);

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
			GimmickDestroy();
		}

		GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &ThisClass::GimmickDestroy, DestroyDelay,
		                                       false);
	}
}

void ATreeGimmick::StartShake()
{
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
	ElapsedTime = 0.f;
	GetWorldTimerManager().ClearTimer(ShakeTimerHandle);

	SetActorLocation(OriginalLocation, false, nullptr, ETeleportType::TeleportPhysics);
}
