#include "DestructibleResourceGimmick.h"

#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Gimmick/Components/DestructibleComponent.h"
#include "Gimmick/Manager/ItemGimmickSubsystem.h"

ADestructibleResourceGimmick::ADestructibleResourceGimmick()
	: bIsActorDestroyOnDestroyAction(true)
	  , DestroyDelay(0.f)
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
}

void ADestructibleResourceGimmick::BeginPlay()
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

	if (IsValid(GeometryCollectionComponent))
	{
		GeometryCollectionComponent->SetVisibility(false);
		GeometryCollectionComponent->SetSimulatePhysics(false);
	}
}

void ADestructibleResourceGimmick::OnGimmickTakeDamage(float DamageAmount, float CurrentHealth)
{
	UE_LOG(LogTemp, Warning, TEXT("Gimmick is damaged / DamageAmount: %.1f / CurrentHealth: %.1f"), DamageAmount,
	       CurrentHealth);
}

void ADestructibleResourceGimmick::OnGimmickDestroy()
{
	if (IsValid(GimmickMeshComponent))
	{
		GimmickMeshComponent->SetVisibility(false);
		GimmickMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		GeometryCollectionComponent->SetVisibility(true);
		GeometryCollectionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GeometryCollectionComponent->SetSimulatePhysics(true);
	}
	
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

void ADestructibleResourceGimmick::DelayedDestroy()
{
	UE_LOG(LogTemp, Warning, TEXT("Gimmick is destroyed"));

	Destroy();
}
