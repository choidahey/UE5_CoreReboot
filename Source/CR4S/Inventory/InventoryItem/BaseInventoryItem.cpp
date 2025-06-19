#include "BaseInventoryItem.h"

#include "CR4S.h"
#include "Character/Characters/PlayerCharacter.h"
#include "Game/System/WorldTimeManager.h"
#include "Inventory/Components/BaseInventoryComponent.h"
#include "Inventory/Components/PlayerInventoryComponent.h"
#include "Kismet/GameplayStatics.h"

UBaseInventoryItem::UBaseInventoryItem()
	: bUsePassiveEffect(false),
	  CurrentStackCount(0)
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

	const UWorld* World = GetWorld();
	if (CR4S_VALIDATE(LogInventoryItem, IsValid(World)))
	{
		WorldTimeManager = World->GetSubsystem<UWorldTimeManager>();
	}
	
	if (bUsePassiveEffect)
	{
		StartPassiveEffect();
	}
}

void UBaseInventoryItem::UseItem(const int32 Index)
{
	USoundBase* UseSound = InventoryItemData.ItemInfoData.UseSound;
	if (IsValid(UseSound))
	{
		UGameplayStatics::PlaySound2D(GetWorld(), UseSound);
	}
}

void UBaseInventoryItem::StartPassiveEffect()
{
	if (IsValid(WorldTimeManager) &&
		!WorldTimeManager->OnWorldTimeUpdated.IsAlreadyBound(this, &ThisClass::HandlePassiveEffect))
	{
		WorldTimeManager->OnWorldTimeUpdated.AddUniqueDynamic(this, &ThisClass::HandlePassiveEffect);
	}
}

void UBaseInventoryItem::HandlePassiveEffect(int64 NewPlayTime)
{
	// CR4S_Log(LogInventory, Warning, TEXT("Begin"));
}


void UBaseInventoryItem::EndPassiveEffect()
{
	if (IsValid(WorldTimeManager) &&
		WorldTimeManager->OnWorldTimeUpdated.IsAlreadyBound(this, &ThisClass::HandlePassiveEffect))
	{
		WorldTimeManager->OnWorldTimeUpdated.RemoveDynamic(this, &ThisClass::HandlePassiveEffect);
	}
}

void UBaseInventoryItem::SetCurrentStackCount(const int32 NewStackCount)
{
	CurrentStackCount = FMath::Clamp(NewStackCount, 0, InventoryItemData.ItemInfoData.MaxStackCount);
}
