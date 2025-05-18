#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "InventorySystemComponent.generated.h"

struct FBaseItemData;
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

	FInventoryItem(const int32 InSlotIndex
	               , const FName& InRowName
	               , UTexture2D* InIcon
	               , const int32 InCount)
		: SlotIndex(InSlotIndex)
		  , RowName(InRowName)
		  , Icon(InIcon)
		  , Count(InCount)
	{
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
	void RemoveItemAt(const int32 Index);
	UFUNCTION()
	void SortInventoryItems();
	bool SwapItems(const int32 FromIndex, const int32 ToIndex);
	void MergeItems(const int32 FromIndex, const int32 ToIndex);

	const FInventoryItem* GetItemDataByIndex(const int32 Index) const;
	
private:
	void GetInventoryItemsAndEmptySlots(const FName& InRowName,
	                                    TArray<FInventoryItem*>& OutSameItems,
	                                    TArray<FInventoryItem*>& OutEmptySlots);
	void SpawnRemainingItems(const FName& ItemRowName, const int32 Count) const;

	const FBaseItemData* FindItemDataFromDataTable(const FName& RowName) const;

	UPROPERTY(EditDefaultsOnly, Category = "InventorySystem", meta = (ClampMin = "1"))
	int32 MaxInventorySlot;

	UPROPERTY(VisibleAnywhere, Category = "InventorySystem")
	TArray<FInventoryItem> InventoryItems;

	UPROPERTY()
	TObjectPtr<UItemGimmickSubsystem> ItemGimmickSubsystem;

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
