#include "DestructibleGimmick.h"

#include "BaseDestructObject.h"
#include "CR4S.h"
#include "Gimmick/Components/DestructibleComponent.h"
#include "Gimmick/Data/GimmickData.h"
#include "Gimmick/Manager/ItemGimmickSubsystem.h"
#include "Inventory/Components/PlayerInventoryComponent.h"

ADestructibleGimmick::ADestructibleGimmick()
	: DestroyDelay(1.f),
	  ToolBonusDamageMultiplier(2.f),
	  bCanShake(true),
	  ShakeDuration(0.5f),
	  ShakeInterval(0.02f),
	  ShakeIntensity(2.5f),
	  OriginalLocation(FVector::ZeroVector),
	  ElapsedTime(0.f)
{
	PrimaryActorTick.bCanEverTick = false;

	DestructibleComponent = CreateDefaultSubobject<UDestructibleComponent>(TEXT("DestructibleComponent"));
}

void ADestructibleGimmick::BeginPlay()
{
	Super::BeginPlay();

	if (CR4S_VALIDATE(LogGimmick, IsValid(DestructibleComponent)))
	{
		DestructibleComponent->OnTakeDamage.BindDynamic(this, &ThisClass::OnGimmickTakeDamage);
		DestructibleComponent->OnDestroy.BindDynamic(this, &ThisClass::OnGimmickDestroy);

		if (CR4S_VALIDATE(LogGimmick, IsValid(ItemGimmickSubsystem)))
		{
			if (const FGimmickInfoData* GimmickInfoData = ItemGimmickSubsystem->
				FindGimmickInfoData(GetGimmickDataRowName()))
			{
				DestructibleComponent->SetMaxHealth(GimmickInfoData->GimmickMaxHealth);
			}
		}
	}

	OriginalLocation = GetActorLocation();
}

float ADestructibleGimmick::TakeDamage(const float DamageAmount, struct FDamageEvent const& DamageEvent,
                                       AController* EventInstigator, AActor* DamageCauser)
{
	float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (!CR4S_VALIDATE(LogGimmick, IsValid(DestructibleComponent)))
	{
		return 0.f;
	}

	const UPlayerInventoryComponent* PlayerInventoryComponent
		= DamageCauser->FindComponentByClass<UPlayerInventoryComponent>();
	if (IsValid(PlayerInventoryComponent) && ToolBonusDamageTag == PlayerInventoryComponent->GetHeldToolTag())
	{
		Damage *= ToolBonusDamageMultiplier;
	}

	DestructibleComponent->TakeDamage(DamageCauser, Damage);

	return Damage;
}

void ADestructibleGimmick::OnGimmickTakeDamage(AActor* DamageCauser, const float DamageAmount,
                                               const float CurrentHealth)
{
	CR4S_Log(LogGimmick, Warning, TEXT("Gimmick is damaged / DamageAmount: %.1f / CurrentHealth: %.1f"), DamageAmount,
	         CurrentHealth);

	if (bCanShake)
	{
		StartShake();
	}
}

void ADestructibleGimmick::OnGimmickDestroy(AActor* DamageCauser)
{
	GetResources(DamageCauser);

	Destroy();
	
	if (CR4S_VALIDATE(LogGimmick, DestructObjectClass))
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride
			= ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		GetWorld()->SpawnActor<ABaseDestructObject>(DestructObjectClass,
		                                            GetActorLocation(),
		                                            GetActorRotation(),
		                                            SpawnParameters);
	}
}

void ADestructibleGimmick::StartShake()
{
	GetWorldTimerManager().SetTimer(
		ShakeTimerHandle,
		this,
		&ThisClass::PerformShake,
		ShakeInterval,
		true
	);
}

void ADestructibleGimmick::PerformShake()
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

void ADestructibleGimmick::StopShake()
{
	ElapsedTime = 0.f;
	GetWorldTimerManager().ClearTimer(ShakeTimerHandle);

	SetActorLocation(OriginalLocation, false, nullptr, ETeleportType::TeleportPhysics);
}
