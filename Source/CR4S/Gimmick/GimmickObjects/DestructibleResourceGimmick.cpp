#include "DestructibleResourceGimmick.h"

#include "Character/CharacterController.h"
#include "Character/Characters/PlayerCharacter.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Gimmick/Components/DestructibleComponent.h"
#include "Gimmick/Manager/ItemGimmickSubsystem.h"
#include "Inventory/InventorySystemComponent.h"

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

float ADestructibleResourceGimmick::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	const float Damage =Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (!IsValid(DestructibleComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("DestructibleComponent is not valid"));
		return 0.f;
	}
	
	DestructibleComponent->TakeDamage(DamageCauser, Damage);
	
	return Damage;
}

void ADestructibleResourceGimmick::OnGimmickTakeDamage(const float DamageAmount, const float CurrentHealth)
{
	UE_LOG(LogTemp, Warning, TEXT("Gimmick is damaged / DamageAmount: %.1f / CurrentHealth: %.1f"), DamageAmount,
	       CurrentHealth);
}

void ADestructibleResourceGimmick::OnGimmickDestroy()
{
	if (IsValid(GimmickMeshComponent)
		&& IsValid(GeometryCollectionComponent)
		&& IsValid(GeometryCollectionComponent->GetRestCollection()))
	{
		GimmickMeshComponent->SetVisibility(false);
		GimmickMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		GeometryCollectionComponent->SetVisibility(true);
		GeometryCollectionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GeometryCollectionComponent->SetSimulatePhysics(true);
	}

	GetResourceItem();

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

void ADestructibleResourceGimmick::GetResourceItem() const
{
	if (!IsValid(DestructibleComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("DestructibleComponent is not valid"));
		return;
	}

	const AActor* DamageCauser = Cast<AActor>(DestructibleComponent->GetLastDamageCauser());
	if (!IsValid(DamageCauser))
	{
		UE_LOG(LogTemp, Warning, TEXT("DamageCauser is not valid"));
		return;
	}

	UInventorySystemComponent* InventorySystem
		= DamageCauser->FindComponentByClass<UInventorySystemComponent>();
	if (!IsValid(InventorySystem))
	{
		UE_LOG(LogTemp, Warning, TEXT("InventorySystem is not valid"));
		return;
	}

	const UItemGimmickSubsystem* ItemGimmickSubsystem = GetGameInstance()->GetSubsystem<UItemGimmickSubsystem>();
	if (!IsValid(ItemGimmickSubsystem))
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemGimmickSubsystem is not valid"));
		return;
	}

	if (const FBaseGimmickData* GimmickData = ItemGimmickSubsystem->FindGimmickData(GetGimmickDataRowName()))
	{
		for (const auto& [RowName, Count] : GimmickData->ResourceItemDataList)
		{
			const FBaseItemData* ItemData = ItemGimmickSubsystem->FindItemData(RowName);
			if (!ItemData)
			{
				UE_LOG(LogTemp, Warning, TEXT("%s is not found in ItemData"), *RowName.ToString());
				continue;
			}

			const FAddItemResult Result
				= InventorySystem->AddItem(FInventoryItem(RowName, ItemData->Icon.Get(), Count));

			UE_LOG(LogTemp, Warning, TEXT("Success: %d / AddCount: %d / RemainingCount: %d")
			       , Result.Success, Result.AddedCount, Result.RemainingCount);
		}
	}
}
