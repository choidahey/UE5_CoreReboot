#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Gimmick/Data/ItemData.h"

#include "InventorySystemComponent.generated.h"

class ASurvivalHUD;
class UBaseInventoryItem;
class UItemGimmickSubsystem;
class UInventoryWidget;

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
class CR4S_API UInventorySystemComponent : public UActorComponent
{
	GENERATED_BODY()

#pragma region UActorComponent Override

public:
	UInventorySystemComponent();

	virtual void BeginPlay() override;

#pragma endregion

#pragma region InventorySystem

public:
	FAddItemResult AddItem(FName RowName, int32 Count);
	/**
	 * - If `RemoveCount` > 0: decreases the item count in that slot by `RemoveCount`.
	 * - If `RemoveCount` == 0 (default): clears the slot entirely (sets count to zero).
	 */
	void RemoveItemAtIndex(const int32 Index, const int32 RemoveCount = 0);
	UFUNCTION()
	void SortInventoryItems();
	bool SwapItems(UBaseInventoryItem* FromItem, UBaseInventoryItem* ToItem) const;
	void MergeItems(UBaseInventoryItem* FromItem, UBaseInventoryItem* ToItem) const;

	UBaseInventoryItem* GetItemDataByIndex(const int32 Index) const;

	FORCEINLINE int32 GetMaxInventorySlot() const { return MaxInventorySlot; }
	FORCEINLINE FName GetCurrentHeldItemName() const { return CurrentHeldItemName; }

protected:
	int32 MaxInventorySlot;

private:
	void GetInventoryItemsAndEmptySlots(const FName& InRowName,
	                                    TArray<UBaseInventoryItem*>& OutSameItems,
	                                    TArray<UBaseInventoryItem*>& OutEmptySlots);
	void SpawnRemainingItems(const FName& ItemRowName, const int32 Count) const;

	const FItemInfoData* FindItemDataFromDataTable(const FName& RowName) const;

	UPROPERTY(VisibleAnywhere, Category = "InventorySystem")
	TArray<TObjectPtr<UBaseInventoryItem>> InventoryItems;

	UPROPERTY()
	TObjectPtr<UItemGimmickSubsystem> ItemGimmickSubsystem;

	UPROPERTY(EditAnywhere, Category = "InventorySystem")
	FName CurrentHeldItemName;

	UPROPERTY()
	TObjectPtr<AActor> OwnerActor;
	UPROPERTY()
	TObjectPtr<ASurvivalHUD> SurvivalHUD;

#pragma endregion

#pragma region InventoryWidget

public:
	UFUNCTION(BlueprintCallable, Category = "InventorySystem|InventoryWidget")
	void OpenInventory() const;

	UFUNCTION(BlueprintCallable, Category = "InventorySystem|InventoryWidget")
	void CloseInventory();

private:
	bool CreateInventoryWidget();

	UPROPERTY(EditDefaultsOnly, Category = "InventoryWidget")
	TSubclassOf<UInventoryWidget> InventoryWidgetClass;
	UPROPERTY(VisibleAnywhere, Category = "InventoryWidget")
	TObjectPtr<UInventoryWidget> InventoryWidgetInstance;

#pragma endregion

#pragma region Delegate

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemSlotChanged, UBaseInventoryItem*, Item);

	FOnItemSlotChanged OnItemSlotChanged;

private:
	void NotifyItemSlotChanged(const int32 ItemSlotIndex) const;
	void NotifyItemSlotsChanged(const TArray<int32>& ChangedItemSlots) const;

#pragma endregion
};
