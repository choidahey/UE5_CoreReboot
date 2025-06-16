#pragma once

#include "CoreMinimal.h"
#include "Gimmick/Data/ItemData.h"
#include "UObject/Object.h"

#include "BaseInventoryItem.generated.h"

class UPlayerInventoryComponent;
class UPlayerCharacterStatusComponent;
class UBaseInventoryComponent;
class APlayerCharacter;

USTRUCT(BlueprintType)
struct FInventoryItemData
{
	GENERATED_BODY()

	FInventoryItemData()
	{
	}

	FInventoryItemData(const FName InRowName, const FItemInfoData& InItemInfoData)
		: RowName(InRowName),
		  ItemInfoData(InItemInfoData)
	{
	}

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
	                               const FInventoryItemData& NewInventoryItemData, const int32 StackCount = 0);

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

	virtual void HandlePassiveEffect();

#pragma endregion

#pragma region Data

public:
	void SetCurrentStackCount(const int32 NewStackCount);

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
