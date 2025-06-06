#include "BaseInventoryItem.h"

#include "CR4S.h"
#include "Character/Characters/PlayerCharacter.h"
#include "Inventory/Components/BaseInventoryComponent.h"
#include "Inventory/Components/PlayerInventoryComponent.h"
#include "Kismet/GameplayStatics.h"

UBaseInventoryItem::UBaseInventoryItem()
	: CurrentStackCount(0)
{
}

void UBaseInventoryItem::InitInventoryItem(UBaseInventoryComponent* NewInventoryComponent,
											   const FInventoryItemData& NewInventoryItemData, const int32 StackCount)
{
	UpdateInventoryItem(NewInventoryComponent);

	if (IsValid(InventoryComponent))
	{
		PlayerInventoryComponent = Cast<UPlayerInventoryComponent>(InventoryComponent);
		OwnerPlayer = Cast<APlayerCharacter>(InventoryComponent->GetOwner());

		if (IsValid(OwnerPlayer))
		{
			PlayerStatusComponent = OwnerPlayer->FindComponentByClass<UPlayerCharacterStatusComponent>();
		}
		else
		{
			PlayerStatusComponent = nullptr;
		}
	}
	
	InventoryItemData = NewInventoryItemData;
	CurrentStackCount = StackCount;
}

void UBaseInventoryItem::UseItem(const int32 Index)
{
	USoundBase* UseSound = InventoryItemData.ItemInfoData.UseSound;
	if (IsValid(UseSound))
	{
		UGameplayStatics::PlaySound2D(GetWorld(), UseSound);
	}
}

void UBaseInventoryItem::HandlePassiveEffect()
{
	CR4S_Log(LogInventory, Warning, TEXT("Begin"));
}

void UBaseInventoryItem::SetCurrentStackCount(const int32 NewStackCount)
{
	CurrentStackCount = FMath::Clamp(NewStackCount, 0, InventoryItemData.ItemInfoData.MaxStackCount);
}
