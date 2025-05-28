#include "ItemPouchGimmick.h"

#include "CR4S.h"
#include "Gimmick/Components/InteractableComponent.h"
#include "Inventory/Components/BaseInventoryComponent.h"
#include "Inventory/Components/PlayerInventoryComponent.h"
#include "Inventory/UI/InventoryType.h"

AItemPouchGimmick::AItemPouchGimmick()
{
	PrimaryActorTick.bCanEverTick = false;

	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));

	InventoryComponent = CreateDefaultSubobject<UBaseInventoryComponent>(TEXT("InventoryComponent"));
	InventoryComponent->SetMaxInventorySlot(100);
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

	PlayerInventoryComponent->OpenOtherInventoryWidget(EInventoryType::Storage, InventoryComponent);
}

void AItemPouchGimmick::InitItemPouch(const TMap<FName, int32>& RemainingItems) const
{
	if (CR4S_VALIDATE(LogGimmick, IsValid(InventoryComponent)))
	{
		InventoryComponent->AddItems(RemainingItems);
	}
}
