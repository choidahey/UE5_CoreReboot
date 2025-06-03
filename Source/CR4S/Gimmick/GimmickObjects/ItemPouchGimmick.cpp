#include "ItemPouchGimmick.h"

#include "CR4S.h"
#include "Gimmick/Components/InteractableComponent.h"
#include "Inventory/Components/BaseInventoryComponent.h"
#include "Inventory/Components/PlayerInventoryComponent.h"
#include "Inventory/InventoryType.h"

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

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (IsValid(PlayerController))
	{
		APawn* PlayerPawn = PlayerController->GetPawn();
		if (IsValid(PlayerController->GetPawn()))
		{
			PlayerInventoryComponent = PlayerPawn->FindComponentByClass<UPlayerInventoryComponent>();
		}
	}
}

void AItemPouchGimmick::OnGimmickInteracted(AActor* Interactor)
{
	if (!CR4S_VALIDATE(LogGimmick, IsValid(Interactor)) ||
		!CR4S_VALIDATE(LogGimmick, IsValid(InventoryComponent)))
	{
		return;
	}

	if (!CR4S_VALIDATE(LogGimmick, IsValid(PlayerInventoryComponent)))
	{
		return;
	}

	if (IsValid(PlayerInventoryComponent))
	{
		InventoryComponent->OnOccupiedSlotsChanged.BindDynamic(this, &ThisClass::DestroyItemPouch);

		PlayerInventoryComponent->OpenOtherInventoryWidget(EInventoryType::ItemPouch, InventoryComponent);
	}
}

void AItemPouchGimmick::InitItemPouch(const TMap<FName, int32>& RemainingItems) const
{
	if (CR4S_VALIDATE(LogGimmick, IsValid(InventoryComponent)))
	{
		InventoryComponent->AddItems(RemainingItems);
	}
}

void AItemPouchGimmick::DestroyItemPouch(const int32 NumOccupiedSlots)
{
	if (NumOccupiedSlots == 0)
	{
		PlayerInventoryComponent->CloseInventoryWidget();

		Destroy();
	}
}
