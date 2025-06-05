#pragma once

#include "CoreMinimal.h"
#include "Gimmick/Data/ItemData.h"
#include "UObject/Object.h"

#include "BaseInventoryItem.generated.h"

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

	UPROPERTY(VisibleAnywhere)
	FName RowName;
	UPROPERTY(VisibleAnywhere)
	FItemInfoData ItemInfoData;

	bool IsStackableItem() const
	{
		return ItemInfoData.MaxStackCount > 1;
	}
};

UCLASS()
class CR4S_API UBaseInventoryItem : public UObject
{
	GENERATED_BODY()

#pragma region UObject Override

public:
	UBaseInventoryItem();

#pragma endregion

#pragma region UseItem

public:
	virtual void UseItem();

#pragma endregion

#pragma region Data

public:
	virtual void InitInventoryItemData(const FInventoryItemData& NewInventoryItemData,
	                                   const int32 StackCount = 0);
	void SetCurrentStackCount(const int32 NewStackCount);

	FORCEINLINE const FInventoryItemData* GetInventoryItemData() const { return &InventoryItemData; }
	FORCEINLINE bool IsStackableItem() const { return InventoryItemData.IsStackableItem(); }
	FORCEINLINE int32 GetMaxStackCount() const { return InventoryItemData.ItemInfoData.MaxStackCount; }
	FORCEINLINE int32 GetCurrentStackCount() const { return CurrentStackCount; }
	FORCEINLINE int32 IsEmpty() const { return CurrentStackCount <= 0; }

private:
	UPROPERTY(VisibleAnywhere, Category = "InventoryItem")
	FInventoryItemData InventoryItemData;
	UPROPERTY(VisibleAnywhere, Category = "InventoryItem")
	int32 CurrentStackCount;

#pragma endregion

#pragma region Delegate

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemUsed);

	FOnItemUsed OnItemUsed;

#pragma endregion
};
