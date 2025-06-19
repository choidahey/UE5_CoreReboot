#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"

#include "BaseInventoryComponent.generated.h"

class UInventoryFilterData;
class UBaseInventoryWidget;
class UQuickSlotBarWidget;
class ASurvivalHUD;
class UBaseInventoryItem;
class UItemGimmickSubsystem;

USTRUCT(BlueprintType)
struct FAddItemResult
{
	GENERATED_BODY()

	FAddItemResult()
		: bSuccess(false),
		  AddedCount(0),
		  RemainingCount(0)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSuccess;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AddedCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
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

#pragma region Initialize

public:
	void InitInventory();

#pragma endregion

#pragma region InventorySystem

public:
	UFUNCTION(BlueprintCallable, Category = "InventoryComponent")
	void AddItems(const TMap<FName, int32>& Items);
	UFUNCTION(BlueprintCallable, Category = "InventoryComponent")
	virtual FAddItemResult AddItem(FName RowName, int32 Count);

	UFUNCTION(BlueprintCallable, Category = "InventoryComponent")
	void RemoveItemByRowName(const FName RowName, const int32 Count);
	UFUNCTION(BlueprintCallable, Category = "InventoryComponent")
	void RemoveAllItemByRowName(const FName RowName);
	UFUNCTION(BlueprintCallable, Category = "InventoryComponent")
	void RemoveItemByIndex(const int32 Index, const int32 Count = -1);

	void SortInventoryItems();
	UBaseInventoryItem* GetInventoryItemByIndex(const int32 Index) const;

	UFUNCTION(BlueprintCallable, Category = "InventoryComponent")
	int32 GetItemCountByRowName(const FName RowName) const;

	void SwapItem(UBaseInventoryComponent* FromInventoryComponent, const int32 FromItemIndex, const int32 ToItemIndex);
	void MergeItem(UBaseInventoryComponent* FromInventoryComponent, const int32 FromItemIndex, const int32 ToItemIndex);
	static void AveragingFreshness(UBaseInventoryItem* FromItem, UBaseInventoryItem* ToItem);

	FORCEINLINE const TArray<TObjectPtr<UBaseInventoryItem>>& GetInventoryItems() const { return InventoryItems; }

	UFUNCTION(BlueprintCallable, Category = "InventoryComponent")
	FORCEINLINE void GetInventoryItems(TArray<UBaseInventoryItem*>& OutInventoryItems) const
	{
		OutInventoryItems = InventoryItems;
	}

	UFUNCTION(BlueprintCallable, Category = "InventoryComponent")
	void SetInventoryItems(const TArray<UBaseInventoryItem*>& NewInventoryItems);

	FORCEINLINE int32 GetMaxInventorySize() const { return MaxInventorySize; }

	FORCEINLINE void SetMaxInventorySize(const int32 InMaxInventorySlot) { MaxInventorySize = InMaxInventorySlot; }
	FORCEINLINE void AddOccupiedSlot(const int32 SlotIndex)
	{
		OccupiedSlots.Add(SlotIndex);
		OnOccupiedSlotsChanged.ExecuteIfBound(OccupiedSlots.Num());
	}

	FORCEINLINE void RemoveOccupiedSlot(const int32 SlotIndex)
	{
		OccupiedSlots.Remove(SlotIndex);
		OnOccupiedSlotsChanged.ExecuteIfBound(OccupiedSlots.Num());
	}

	int32 GetUseSlotCount();
	FORCEINLINE int32 GetNumOccupiedSlots() const { return OccupiedSlots.Num(); }

	FORCEINLINE const FText& GetInventoryTitleText() const { return InventoryTitleText; }
	FORCEINLINE void SetInventoryTitleText(const FText& NewInventoryTitleText)
	{
		InventoryTitleText = NewInventoryTitleText;
	}

protected:
	void GetSameItemSlotsAndEmptySlots(const FName& InRowName,
	                                   const TArray<TObjectPtr<UBaseInventoryItem>>& SlotBox,
	                                   TArray<int32>& OutSameItemsIndex,
	                                   TArray<int32>& OutEmptySlotsIndex);

	void StackItemsAndFillEmptySlots(FName RowName,
	                                 int32 Count,
	                                 FAddItemResult& Result,
	                                 TSet<int32>& ChangedItemSlots);

	UBaseInventoryItem* CreateInventoryItem(const FGameplayTagContainer& ItemTags);

	UPROPERTY(EditDefaultsOnly, Category = "InventorySystem")
	int32 MaxInventorySize;

	UPROPERTY(VisibleAnywhere, Category = "InventorySystem")
	TSet<int32> OccupiedSlots;
	UPROPERTY(VisibleAnywhere, Category = "InventorySystem")
	TArray<TObjectPtr<UBaseInventoryItem>> InventoryItems;

	UPROPERTY(EditDefaultsOnly, Category = "InventorySystem")
	FText InventoryTitleText;

	UPROPERTY()
	TObjectPtr<UItemGimmickSubsystem> ItemGimmickSubsystem;
	UPROPERTY()
	TObjectPtr<AActor> OwnerActor;

#pragma endregion

#pragma region Filter

public:
	bool IsItemAllowedByFilter(const FGameplayTagContainer& ItemTags) const;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Filter")
	TObjectPtr<UInventoryFilterData> FilterData;

#pragma endregion

#pragma region Delegate

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemSlotChanged, const int32, SlotIndex, UBaseInventoryItem*, Item);

	UPROPERTY(BlueprintAssignable, Category = "InventoryComponent|Delegates")
	FOnItemSlotChanged OnItemSlotChanged;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryClosed);

	UPROPERTY(BlueprintAssignable, Category = "InventoryComponent|Delegates")
	FOnInventoryClosed OnInventoryClosed;

	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnOccupiedSlotsChanged, const int32, NumOccupiedSlots);

	FOnOccupiedSlotsChanged OnOccupiedSlotsChanged;

protected:
	void NotifyInventoryItemChanged(const int32 ItemIndex) const;
	void NotifyInventoryItemsChanged(const TArray<int32>& ChangedItemIndexes) const;

#pragma endregion
};
