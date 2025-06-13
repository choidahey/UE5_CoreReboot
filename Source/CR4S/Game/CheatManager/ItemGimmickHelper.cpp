#include "ItemGimmickHelper.h"

#include "Gimmick/Manager/ItemGimmickSubsystem.h"
#include "Inventory/Components/PlayerInventoryComponent.h"

void UItemGimmickHelper::AddItem(const FName RowName, const int32 Count) const
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}
	
	const APlayerController* PlayerController = World->GetFirstPlayerController(); 
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
		FAddItemResult Result = PlayerInventoryComponent->AddItem(RowName, Count);

		if (Result.bSuccess && Result.RemainingCount > 0)
		{
			UItemGimmickSubsystem* ItemGimmickSubsystem = GetWorld()->GetSubsystem<UItemGimmickSubsystem>();
			if (IsValid(ItemGimmickSubsystem))
			{
				ItemGimmickSubsystem->SpawnItemPouch(Pawn, {{RowName, Result.RemainingCount}});
			}
		}
	}
}
