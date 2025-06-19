#pragma once

#include "CoreMinimal.h"
#include "Gimmick/Data/ItemData.h"
#include "UObject/Object.h"

#include "BaseInventoryItem.generated.h"

class UWorldTimeManager;
class UPlayerInventoryComponent;
class UPlayerCharacterStatusComponent;
class UBaseInventoryComponent;
class APlayerCharacter;

USTRUCT(BlueprintType)
struct FInventoryItemData
{
	GENERATED_BODY()

	FInventoryItemData()
		: SlotIndex(0)
	{
	}

	FInventoryItemData(const int32 InSlotIndex,
	                   const FName InRowName,
	                   const FItemInfoData& InItemInfoData)
		: SlotIndex(InSlotIndex),
		  RowName(InRowName),
		  ItemInfoData(InItemInfoData)
	{
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 SlotIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName RowName;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FItemInfoData ItemInfoData;

	bool IsStackableItem() const
	{
		return ItemInfoData.MaxStackCount > 1;
	}
};

UCLASS(BlueprintType)
class CR4S_API UBaseInventoryItem : public UObject
{
	GENERATED_BODY()

#pragma region UObject Override

public:
	UBaseInventoryItem();

#pragma endregion

#pragma region Initialize

public:
	virtual void InitInventoryItem(UBaseInventoryComponent* NewInventoryComponent,
	                               const FInventoryItemData& NewInventoryItemData,
	                               const int32 StackCount = 0);

	FORCEINLINE void UpdateInventoryItem(UBaseInventoryComponent* NewInventoryComponent)
	{
		InventoryComponent = NewInventoryComponent;
	}

protected:
	UPROPERTY()
	TObjectPtr<UBaseInventoryComponent> InventoryComponent;

	UPROPERTY()
	TObjectPtr<APlayerCharacter> OwnerPlayer;

	UPROPERTY()
	TObjectPtr<UPlayerInventoryComponent> PlayerInventoryComponent;

	UPROPERTY()
	TObjectPtr<UPlayerCharacterStatusComponent> PlayerStatusComponent;

#pragma endregion

#pragma region Item Behavior

public:
	virtual void UseItem(int32 Index);

	virtual void StartPassiveEffect();
	UFUNCTION()
	virtual void HandlePassiveEffect(int64 NewPlayTime);
	virtual void EndPassiveEffect();

protected:
	UPROPERTY()
	TObjectPtr<UWorldTimeManager> WorldTimeManager;

	bool bUsePassiveEffect;

#pragma endregion

#pragma region Data

public:
	void SetCurrentStackCount(const int32 NewStackCount);

	FORCEINLINE void ChangeSlotIndex(const int32 NewSlotIndex) { InventoryItemData.SlotIndex = NewSlotIndex; }

	FORCEINLINE const FInventoryItemData* GetInventoryItemData() const { return &InventoryItemData; }
	UFUNCTION(BlueprintCallable, Category = "InventoryItem|Data")
	FORCEINLINE void GetInventoryItemData(FInventoryItemData& OutInventoryItemData) const
	{
		OutInventoryItemData = InventoryItemData;
	}

	FORCEINLINE bool IsStackableItem() const { return InventoryItemData.IsStackableItem(); }
	FORCEINLINE int32 GetMaxStackCount() const { return InventoryItemData.ItemInfoData.MaxStackCount; }
	UFUNCTION(BlueprintCallable, Category = "InventoryItem|Data")
	FORCEINLINE int32 GetCurrentStackCount() const { return CurrentStackCount; }

	FORCEINLINE int32 IsEmpty() const { return CurrentStackCount <= 0; }
	FORCEINLINE const FGameplayTagContainer& GetItemTags() const { return InventoryItemData.ItemInfoData.ItemTags; }

	FORCEINLINE const FText& GetItemName() const { return InventoryItemData.ItemInfoData.Name; }
	FORCEINLINE const FText& GetItemDescription() const { return InventoryItemData.ItemInfoData.Description; }
	FORCEINLINE UTexture2D* GetItemIcon() const { return InventoryItemData.ItemInfoData.Icon; }

	FORCEINLINE void UpdateItemDescription(const FText& NewDescription)
	{
		InventoryItemData.ItemInfoData.Description = NewDescription;
	}

protected:
	UPROPERTY(VisibleAnywhere, Category = "InventoryItem")
	FInventoryItemData InventoryItemData;
	UPROPERTY(VisibleAnywhere, Category = "InventoryItem")
	int32 CurrentStackCount;

#pragma endregion

#pragma region Delegate

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemUsed);

	UPROPERTY(BlueprintAssignable, Category = "InventoryItem|Delegates")
	FOnItemUsed OnItemUsed;

#pragma endregion
};
