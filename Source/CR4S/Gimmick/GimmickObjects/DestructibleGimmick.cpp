#include "DestructibleGimmick.h"

#include "CR4S.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Gimmick/Components/DestructibleComponent.h"
#include "Gimmick/Manager/ItemGimmickSubsystem.h"

ADestructibleGimmick::ADestructibleGimmick()
	: DestroyDelay(0.f),
	  DestroyImpulseRadius(50.f),
	  DestroyImpulseStrength(50.f),
	  bCanShake(true),
	  ShakeDuration(0.5f),
	  ShakeInterval(0.02f),
	  ShakeIntensity(5.0f),
	  OriginalLocation(FVector::ZeroVector),
	  ElapsedTime(0.f)
{
	PrimaryActorTick.bCanEverTick = false;

	DestructibleComponent = CreateDefaultSubobject<UDestructibleComponent>(TEXT("DestructibleComponent"));

	GeometryCollectionComponent
		= CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollectionComponent"));
	GeometryCollectionComponent->SetupAttachment(RootComponent);
	GeometryCollectionComponent->SetVisibility(false);
	GeometryCollectionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GeometryCollectionComponent->SetSimulatePhysics(false);
	GeometryCollectionComponent->DamageThreshold.Init(0.f, 1);
	GeometryCollectionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GeometryCollectionComponent->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);
}

void ADestructibleGimmick::BeginPlay()
{
	Super::BeginPlay();

	if (CR4S_VALIDATE(LogGimmick, IsValid(DestructibleComponent)))
	{
		DestructibleComponent->OnTakeDamage.BindDynamic(this, &ThisClass::OnGimmickTakeDamage);
		DestructibleComponent->OnDestroy.BindDynamic(this, &ThisClass::OnGimmickDestroy);

		const UItemGimmickSubsystem* GimmickSubsystem = GetGameInstance()->GetSubsystem<UItemGimmickSubsystem>();

		if (!CR4S_VALIDATE(LogGimmick, IsValid(GimmickSubsystem)))
		{
			return;
		}

		if (const FGimmickInfoData* GimmickData = GimmickSubsystem->FindGimmickInfoData(GetGimmickDataRowName()))
		{
			DestructibleComponent->SetMaxHealth(GimmickData->GimmickMaxHealth);
		}
	}

	if (CR4S_VALIDATE(LogGimmick, IsValid(GeometryCollectionComponent)))
	{
		GeometryCollectionComponent->SetVisibility(false);
		GeometryCollectionComponent->SetSimulatePhysics(false);
	}

	OriginalLocation = GetActorLocation();
}

float ADestructibleGimmick::TakeDamage(const float DamageAmount, struct FDamageEvent const& DamageEvent,
                                       AController* EventInstigator, AActor* DamageCauser)
{
	const float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (!CR4S_VALIDATE(LogGimmick, IsValid(DestructibleComponent)))
	{
		return 0.f;
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

	if (CR4S_VALIDATE(LogGimmick, IsValid(GimmickMeshComponent)) &&
		CR4S_VALIDATE(LogGimmick, IsValid(GeometryCollectionComponent)) &&
		CR4S_VALIDATE(LogGimmick, IsValid(GeometryCollectionComponent->GetRestCollection())))
	{
		GimmickMeshComponent->SetVisibility(false);
		GimmickMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		GeometryCollectionComponent->SetVisibility(true);
		GeometryCollectionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GeometryCollectionComponent->SetSimulatePhysics(true);
		GeometryCollectionComponent->AddRadialImpulse(
			GeometryCollectionComponent->GetComponentLocation(),
			DestroyImpulseRadius,
			DestroyImpulseStrength,
			RIF_Linear,
			true
		);
	}

	if (DestroyDelay == 0.f)
	{
		GimmickDestroy();
	}

	GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &ThisClass::GimmickDestroy, DestroyDelay, false);
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
