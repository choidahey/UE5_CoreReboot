#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "BaseInventoryComponent.generated.h"

class UBaseInventoryWidget;
class UQuickSlotBarWidget;
class ASurvivalHUD;
class UBaseInventoryItem;
class UItemGimmickSubsystem;

USTRUCT()
struct FAddItemResult
{
	GENERATED_BODY()

	FAddItemResult()
		: Success(false),
		  AddedCount(0),
		  RemainingCount(0)
	{
	}

	bool Success;
	int32 AddedCount;
	int32 RemainingCount;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UBaseInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

#pragma region UActorComponent Override

public:
	UBaseInventoryComponent();

	virtual void BeginPlay() override;

#pragma endregion

#pragma region InventorySystem

public:
	void InitInventorySize();

	void AddItems(const TMap<FName, int32>& Items);
	virtual FAddItemResult AddItem(FName RowName, int32 Count);

	void RemoveItem(const FName RowName, const int32 Count);
	
	void SortInventoryItems();
	UBaseInventoryItem* GetItemDataByIndex(const int32 Index) const;
	int32 GetItemCountByRowName(const FName RowName) const;

	FORCEINLINE const TArray<TObjectPtr<UBaseInventoryItem>>& GetInventoryItems() const { return InventoryItems; }
	
	FORCEINLINE int32 GetMaxInventorySlot() const { return MaxItemSlot; }
	FORCEINLINE void SetMaxInventorySlot(const int32 InMaxInventorySlot) { MaxItemSlot = InMaxInventorySlot; }
	FORCEINLINE void AddOccupiedSlot(const int32 SlotIndex) { OccupiedSlots.Add(SlotIndex); }
	FORCEINLINE void RemoveOccupiedSlot(const int32 SlotIndex) { OccupiedSlots.Remove(SlotIndex); }
	FORCEINLINE int32 GetNumOccupiedSlots() const { return OccupiedSlots.Num(); }

protected:
	void GetSameItemSlotsAndEmptySlots(const FName& InRowName,
	                                   const TArray<TObjectPtr<UBaseInventoryItem>>& SlotBox,
	                                   TArray<UBaseInventoryItem*>& OutSameItems,
	                                   TArray<UBaseInventoryItem*>& OutEmptySlots);

	void StackItemsAndFillEmptySlots(FName RowName,
	                                 int32 Count,
	                                 const TArray<TObjectPtr<UBaseInventoryItem>>& SlotBox,
	                                 FAddItemResult& Result,
	                                 TSet<int32>& ChangedItemSlots);

	UPROPERTY(EditDefaultsOnly, Category = "InventorySystem")
	int32 MaxItemSlot;

	UPROPERTY(VisibleAnywhere, Category = "InventorySystem")
	TSet<int32> OccupiedSlots;
	UPROPERTY(VisibleAnywhere, Category = "InventorySystem")
	TArray<TObjectPtr<UBaseInventoryItem>> InventoryItems;

	UPROPERTY()
	TObjectPtr<UItemGimmickSubsystem> ItemGimmickSubsystem;

	UPROPERTY()
	TObjectPtr<AActor> OwnerActor;

#pragma endregion

#pragma region Delegate

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemSlotChanged, UBaseInventoryItem*, Item);

	FOnItemSlotChanged OnItemSlotChanged;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryClosed);

	FOnInventoryClosed OnInventoryClosed;

private:
	void NotifyInventoryItemChanged(const int32 ItemIndex) const;
	void NotifyInventoryItemsChanged(const TArray<int32>& ChangedItemIndexes) const;

#pragma endregion
};
