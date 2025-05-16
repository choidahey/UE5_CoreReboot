#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "InventorySystemComponent.generated.h"

class UItemGimmickSubsystem;
class UInventoryWidget;

USTRUCT()
struct FInventoryItem
{
	GENERATED_BODY()

	FInventoryItem()
		: RowName("")
		  , Count(0)
	{
	}

	FInventoryItem(const FName& InRowName, const int32 InCount)
		: RowName(InRowName)
		  , Count(InCount)
	{
	}

	UPROPERTY(Editanywhere)
	FName RowName;
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

private:
	void GetInventoryItemsAndEmptySlots(const FName& InRowName,
	                                    TArray<FInventoryItem*>& OutSameItems,
	                                    TArray<FInventoryItem*>& OutEmptySlots);
	void SpawnRemainingItems(const FName& ItemRowName, const int32 Count) const;

	bool SwapItemIndex(const int32 FromIndex, const int32 ToIndex);

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

#pragma endregion

#pragma region Delegate

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemSlotChanged, int32, ItemSlotIndex);
	FOnItemSlotChanged OnItemSlotChanged;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);
	FOnInventoryChanged OnInventoryChanged;

private:
	void NotifyItemSlotChanged(const int32 ItemSlotIndex) const;
	void NotifyItemSlotsChanged(const TArray<int32>& ChangedItemSlots) const;
	
#pragma endregion
};
