#include "HelperBotInventoryItem.h"

#include "CR4S.h"
#include "Character/Characters/PlayerCharacter.h"
#include "FriendlyAI/BaseHelperBot.h"
#include "Game/SaveGame/InventorySaveGame.h"

#define LOCTEXT_NAMESPACE "HelperBotInventoryItem"

void UHelperBotInventoryItem::InitInventoryItem(UBaseInventoryComponent* NewInventoryComponent,
                                                const FInventoryItemData& NewInventoryItemData, const int32 StackCount)
{
	Super::InitInventoryItem(NewInventoryComponent, NewInventoryItemData, StackCount);

	InventoryItemData.ItemType = EInventoryItemType::HelperBot;

	BotNameText = LOCTEXT("BotNameText", "이름");
	HPText = LOCTEXT("HPText", "체력");
}

void UHelperBotInventoryItem::UseItem(const int32 Index)
{
	Super::UseItem(Index);

	const UDataTable* DataTable = GetItemInfoData().DetailData.DataTable;
	if (!CR4S_VALIDATE(LogInventoryItem, IsValid(DataTable)))
	{
		return;
	}

	const UWorld* World = GetWorld();
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
	const FRotator ControlRot = OwnerPlayer->GetControlRotation();
	const FVector ForwardVector = FRotator(0.f, ControlRot.Yaw, 0.f).Vector().GetSafeNormal();
	const FVector OffsetForward = ForwardVector * HelperBotItem->SpawnOffset.X;
	const FVector OffsetUp = FVector(0.f, 0.f, HelperBotItem->SpawnOffset.Z);

	const FVector ActorLocation = OwnerPlayer->GetActorLocation();
	const FVector SpawnLocation = ActorLocation + OffsetForward + OffsetUp;
	FRotator SpawnRotation = (ActorLocation - SpawnLocation).Rotation();
	SpawnRotation.Pitch = 0.f;
	SpawnRotation.Roll = 0.f;

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ABaseHelperBot* HelperBot = GetWorld()->SpawnActor<ABaseHelperBot>(HelperBotItem->HelperBotClass,
	                                                                   SpawnLocation,
	                                                                   SpawnRotation,
	                                                                   SpawnParameters);

	if (IsValid(HelperBot))
	{
		HelperBot->SetIsFromInventory(true);
		HelperBot->SetPickUpData(GetHelperBotData());

		InventoryComponent->RemoveItemByIndex(Index, 1);
	}
	else
	{
		CR4S_Log(LogInventoryItem, Warning, TEXT("Helper Bot Spawn Failed"));
	}
}

void UHelperBotInventoryItem::SetHelperBotData(const FHelperPickUpData& NewHelperBotData)
{
	HelperBotData = NewHelperBotData;

	const FText NewDescription = FText::Format(
		LOCTEXT("ItemDescriptionFormat", "{0}: {1}\n{2}: {3}"),
		BotNameText,
		FText::FromString(HelperBotData.BotName),
		HPText,
		FText::AsNumber(HelperBotData.CurrentHealth)
	);

	InventoryItemData.ItemInfoData.Description = NewDescription;
}

FInventoryItemSaveGame UHelperBotInventoryItem::GetInventoryItemSaveData()
{
	FInventoryItemSaveGame ItemSaveGame = Super::GetInventoryItemSaveData();
	ItemSaveGame.InventoryItemData.ItemType = EInventoryItemType::HelperBot;
	ItemSaveGame.HelperBotItemData = HelperBotData;

	return ItemSaveGame;
}

void UHelperBotInventoryItem::LoadInventoryItemSaveData(UBaseInventoryComponent* NewInventoryComponent,
                                                        const FInventoryItemSaveGame& ItemSaveGame)
{
	Super::LoadInventoryItemSaveData(NewInventoryComponent, ItemSaveGame);

	HelperBotData = ItemSaveGame.HelperBotItemData;
}

#undef LOCTEXT_NAMESPACE
