#include "ItemCheatManager.h"

#include "Inventory/Components/PlayerInventoryComponent.h"

void UItemCheatManager::AddItem(const FName RowName, const int32 Count) const
{
	const APlayerController* PlayerController = GetPlayerController();
	if (!IsValid(PlayerController))
	{
		return;
	}

	const APawn* Pawn = PlayerController->GetPawn();
	if (!IsValid(Pawn))
	{
		return;
	}

	UPlayerInventoryComponent* PlayerInventoryComponent = Pawn->FindComponentByClass<UPlayerInventoryComponent>();
	if (IsValid(PlayerInventoryComponent))
	{
		PlayerInventoryComponent->AddItem(RowName, Count);
	}
}
