#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Gimmick/Data/ItemData.h"

#include "InventorySystemComponent.generated.h"

class UItemGimmickSubsystem;
class UInventoryWidget;

USTRUCT()
struct FInventoryItem
{
	GENERATED_BODY()

	FInventoryItem()
		: SlotIndex(0)
		  , RowName("")
		  , Icon(nullptr)
		  , Count(0)
	{
	}

	FInventoryItem(const FName& InRowName
	               , UTexture2D* InIcon
	               , const int32 InCount)
		: SlotIndex(0)
		  , RowName(InRowName)
		  , Icon(InIcon)
		  , Count(InCount)
	{
	}

	void SwapItem(FInventoryItem& Other)
	{
		Swap(SlotIndex, Other.SlotIndex);
		Swap(*this, Other);
	}

	UPROPERTY()
	int32 SlotIndex;
	UPROPERTY(Editanywhere)
	FName RowName;
	UPROPERTY(EditAnywhere)
	UTexture2D* Icon;
	UPROPERTY(Editanywhere, meta = (ClampMin = "0"))
	int32 Count;
};

USTRUCT()
struct FAddItemResult
{
	GENERATED_BODY()

	FAddItemResult()
		: Success(false)
		  , AddedCount(0)
		  , RemainingCount(0)
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
	FAddItemResult AddItem(const FInventoryItem& InInventoryItem);
	/**
	 * - If `RemoveCount` > 0: decreases the item count in that slot by `RemoveCount`.
	 * - If `RemoveCount` == 0 (default): clears the slot entirely (sets count to zero).
	 */
	void RemoveItemAtIndex(const int32 Index, const int32 RemoveCount = 0);
	UFUNCTION()
	void SortInventoryItems();
	bool SwapItems(const int32 FromIndex, const int32 ToIndex);
	void MergeItems(const int32 FromIndex, const int32 ToIndex);

	const FInventoryItem* GetItemDataByIndex(const int32 Index) const;

	FORCEINLINE FName GetCurrentHeldItemName() const { return CurrentHeldItemName; }

private:
	void GetInventoryItemsAndEmptySlots(const FName& InRowName,
	                                    TArray<FInventoryItem*>& OutSameItems,
	                                    TArray<FInventoryItem*>& OutEmptySlots);
	void SpawnRemainingItems(const FName& ItemRowName, const int32 Count) const;

	const FItemInfoData* FindItemDataFromDataTable(const FName& RowName) const;

	UPROPERTY(EditDefaultsOnly, Category = "InventorySystem", meta = (ClampMin = "1"))
	int32 MaxInventorySlot;

	UPROPERTY(VisibleAnywhere, Category = "InventorySystem")
	TArray<FInventoryItem> InventoryItems;

	UPROPERTY()
	TObjectPtr<UItemGimmickSubsystem> ItemGimmickSubsystem;

	UPROPERTY(EditAnywhere, Category = "InventorySystem")
	FName CurrentHeldItemName;

#pragma endregion

#pragma region InventoryWidget

public:
	UFUNCTION(BlueprintCallable, Category = "InventorySystem|InventoryWidget")
	void OpenInventory();

	UFUNCTION(BlueprintCallable, Category = "InventorySystem|InventoryWidget")
	void CloseInventory();

private:
	void CreateInventoryWidget();

	UPROPERTY(EditDefaultsOnly, Category = "InventoryWidget")
	TSubclassOf<UInventoryWidget> InventoryWidgetClass;
	UPROPERTY(VisibleAnywhere, Category = "InventoryWidget")
	TObjectPtr<UInventoryWidget> InventoryWidgetInstance;

	UPROPERTY()
	TObjectPtr<APlayerController> OwnerPlayerController;

#pragma endregion

#pragma region Delegate

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnItemSlotChanged
	                                              , const int32, Index
	                                              , const FName&, RowName
	                                              , UTexture2D*, IconTexture
	                                              , const int32, Count);

	FOnItemSlotChanged OnItemSlotChanged;

private:
	void NotifyItemSlotChanged(const int32 ItemSlotIndex) const;
	void NotifyItemSlotsChanged(const TArray<int32>& ChangedItemSlots) const;

#pragma endregion
};
