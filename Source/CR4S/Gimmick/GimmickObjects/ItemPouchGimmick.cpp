#include "ItemPouchGimmick.h"

#include "CR4S.h"
#include "Gimmick/Components/InteractableComponent.h"
#include "Inventory/Components/BaseInventoryComponent.h"
#include "Inventory/Components/PlayerInventoryComponent.h"
#include "Inventory/InventoryItem/BaseInventoryItem.h"

AItemPouchGimmick::AItemPouchGimmick()
{
	PrimaryActorTick.bCanEverTick = false;

	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));

	InventoryComponent = CreateDefaultSubobject<UBaseInventoryComponent>(TEXT("InventoryComponent"));
	InventoryComponent->SetMaxInventorySlot(10);
}

void AItemPouchGimmick::BeginPlay()
{
	Super::BeginPlay();

	if (CR4S_VALIDATE(LogGimmick, IsValid(InteractableComponent)))
	{
		InteractableComponent->OnTryInteract.BindDynamic(this, &ThisClass::OnGimmickInteracted);
	}
}

void AItemPouchGimmick::OnGimmickInteracted(AActor* Interactor)
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
}

void AItemPouchGimmick::InitItemPouch(const TArray<UBaseInventoryItem*>& Items) const
{
	if (!CR4S_VALIDATE(LogGimmick, IsValid(InventoryComponent)))
	{
		return;
	}

	const int32 ItemCount = Items.Num();
	InventoryComponent->SetMaxInventorySlot(ItemCount);
	InventoryComponent->InitInventorySize();

	for (const UBaseInventoryItem* Item : Items)
	{
		InventoryComponent->AddItem(Item->GetInventoryItemData()->RowName, Item->GetCurrentStackCount());
	}
}
