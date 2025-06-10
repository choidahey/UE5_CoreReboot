#include "StorageGimmick.h"

#include "CR4S.h"
#include "Gimmick/Components/InteractableComponent.h"
#include "Inventory/InventoryType.h"
#include "Inventory/Components/PlayerInventoryComponent.h"
#include "Inventory/InventoryItem/BaseInventoryItem.h"

AStorageGimmick::AStorageGimmick()
{
	PrimaryActorTick.bCanEverTick = false;

	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));
	InventoryComponent = CreateDefaultSubobject<UBaseInventoryComponent>(TEXT("InventoryComponent"));
	InventoryComponent->SetMaxInventorySize(100);
}

void AStorageGimmick::BeginPlay()
{
	Super::BeginPlay();

	if (CR4S_VALIDATE(LogGimmick, IsValid(InteractableComponent)))
	{
		InteractableComponent->OnTryInteract.AddUniqueDynamic(this, &ThisClass::OnGimmickInteracted);
	}
}

void AStorageGimmick::OnGimmickDestroy(AActor* DamageCauser)
{
	if (!CR4S_VALIDATE(LogGimmick, IsValid(DamageCauser)) ||
		!CR4S_VALIDATE(LogGimmick, IsValid(InventoryComponent)))
	{
		return;
	}

	UPlayerInventoryComponent* PlayerInventoryComponent = DamageCauser->FindComponentByClass<
		UPlayerInventoryComponent>();
	if (!CR4S_VALIDATE(LogGimmick, IsValid(PlayerInventoryComponent)))
	{
		return;
	}

	const TArray<TObjectPtr<UBaseInventoryItem>>& StorageItems = InventoryComponent->GetInventoryItems();

	TMap<FName, int32> Items;
	for (const auto& Item : StorageItems)
	{
		if (IsValid(Item))
		{
			Items.FindOrAdd(Item->GetInventoryItemData()->RowName) += Item->GetCurrentStackCount();
		}
	}

	PlayerInventoryComponent->AddItems(Items);

	Super::OnGimmickDestroy(DamageCauser);
}

void AStorageGimmick::OnGimmickInteracted(AActor* Interactor)
{
	if (!CR4S_VALIDATE(LogGimmick, IsValid(Interactor)) ||
		!CR4S_VALIDATE(LogGimmick, IsValid(InventoryComponent)))
	{
		return;
	}

	UPlayerInventoryComponent* PlayerInventoryComponent = Interactor->FindComponentByClass<UPlayerInventoryComponent>();
	if (!CR4S_VALIDATE(LogGimmick, IsValid(PlayerInventoryComponent)))
	{
		return;
	}

	PlayerInventoryComponent->OpenOtherInventoryWidget(EInventoryType::Storage, InventoryComponent);
}
