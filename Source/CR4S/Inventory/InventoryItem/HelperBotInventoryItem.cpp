#include "HelperBotInventoryItem.h"

#include "CR4S.h"
#include "Character/Characters/PlayerCharacter.h"
#include "FriendlyAI/BaseHelperBot.h"

void UHelperBotInventoryItem::UseItem(const int32 Index)
{
	Super::UseItem(Index);

	const UDataTable* DataTable = GetItemInfoData().DetailData.DataTable;
	if (!CR4S_VALIDATE(LogInventoryItem, IsValid(DataTable)))
	{
		return;
	}

	UWorld* World = GetWorld();
	const FHelperBotItemData* HelperBotItem
		= DataTable->FindRow<FHelperBotItemData>(GetItemRowName(), TEXT(""));
	if (!CR4S_VALIDATE(LogInventoryItem, IsValid(World)) ||
		!CR4S_VALIDATE(LogInventoryItem, HelperBotItem) ||
		!CR4S_VALIDATE(LogInventoryItem, IsValid(OwnerPlayer)) ||
		!CR4S_VALIDATE(LogInventoryItem, HelperBotItem->HelperBotClass) ||
		!CR4S_VALIDATE(LogInventory, IsValid(InventoryComponent)))
	{
		return;
	}

	FVector SpawnLocation = OwnerPlayer->GetActorLocation();
	SpawnLocation += OwnerPlayer->GetActorForwardVector() * 100.f;
	const FRotator SpawnRotation = OwnerPlayer->GetActorForwardVector().Rotation();

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	GetWorld()->SpawnActor<ABaseHelperBot>(HelperBotItem->HelperBotClass,
	                       SpawnLocation,
	                       SpawnRotation,
	                       SpawnParameters);
	

	InventoryComponent->RemoveItemByIndex(Index, 1);
}
