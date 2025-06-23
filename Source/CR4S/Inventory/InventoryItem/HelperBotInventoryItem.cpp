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
