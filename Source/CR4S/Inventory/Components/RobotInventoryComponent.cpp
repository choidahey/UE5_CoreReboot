#include "RobotInventoryComponent.h"

#include "CR4S.h"
#include "PlayerInventoryComponent.h"
#include "Character/Characters/PlayerCharacter.h"
#include "Inventory/InventoryItem/BaseInventoryItem.h"

URobotInventoryComponent::URobotInventoryComponent()
	: PlayerInventoryComponent(nullptr)
{
	PrimaryComponentTick.bCanEverTick = false;

	MaxInventorySize = 9;
}

FAddItemResult URobotInventoryComponent::AddItem(const FName RowName, const int32 Count, UBaseInventoryItem* OriginItem)
{
	if (CR4S_VALIDATE(LogInventory, IsValid(PlayerInventoryComponent)))
	{
		return PlayerInventoryComponent->AddItem(RowName, Count);
	}
	
	FAddItemResult Result;
	Result.RemainingCount = Count;
	return Result;
}

void URobotInventoryComponent::UpdatePlayerInventoryComponent(const APlayerCharacter* MountedCharacter)
{
	if (IsValid(MountedCharacter))
	{
		PlayerInventoryComponent = MountedCharacter->FindComponentByClass<UPlayerInventoryComponent>();
	}
	else
	{
		PlayerInventoryComponent = nullptr;
	}
}

bool URobotInventoryComponent::CanAssembleRobot() const
{
	const int32 RequirementsTagCount = RequirementTagQuery.GetGameplayTagArray().Num();

	if (RequirementsTagCount == 0)
	{
		return true;
	}
	
	int32 MatchesTagCount = 0;
	for (const UBaseInventoryItem* InventoryItem : InventoryItems)
	{
		if (IsValid(InventoryItem) && RequirementTagQuery.Matches(InventoryItem->GetItemTags()))
		{
			MatchesTagCount++;
		}
	}

	return RequirementsTagCount <= MatchesTagCount;
}
