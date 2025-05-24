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
		: ItemType(EItemType::Resource),
		  MaxStackCount(0)
	{
	}

	FInventoryItemData(const EItemType InItemType, const FName InRowName, const int32 InMaxStackCount)
		: ItemType(InItemType),
		  RowName(InRowName),
		  MaxStackCount(InMaxStackCount)
	{
	}

	UPROPERTY(VisibleAnywhere)
	EItemType ItemType;
	UPROPERTY(VisibleAnywhere)
	FName RowName;
	UPROPERTY(VisibleAnywhere)
	int32 MaxStackCount;
};

UCLASS()
class CR4S_API UBaseInventoryItem : public UObject
{
	GENERATED_BODY()

#pragma region UObject Override

public:
	UBaseInventoryItem();

#pragma endregion

#pragma region Initalize

public:
	void InitInventoryItem(AActor* NewOwner, int32 NewSlotIndex);

#pragma endregion

#pragma region UseItem

public:
	virtual void UseItem();

#pragma endregion

#pragma region Data

public:
	void SetInventoryItemData(const FInventoryItemData& NewInventoryItemData, UTexture2D* NewIcon, const int32 StackCount = 0);
	void SetCurrentStackCount(const int32 NewStackCount);

	void SwapData(UBaseInventoryItem* OtherItem);

	FORCEINLINE AActor* GetOwner() const { return Owner; }
	FORCEINLINE bool HasItemData() const { return bHasItemData; }
	FORCEINLINE int32 GetSlotIndex() const { return SlotIndex; }
	FORCEINLINE const FInventoryItemData* GetInventoryItemData() const { return &InventoryItemData; }
	FORCEINLINE UTexture2D* GetIcon() const { return Icon; }
	FORCEINLINE int32 GetCurrentStackCount() const { return CurrentStackCount; }

private:
	UPROPERTY()
	TObjectPtr<AActor> Owner;
	UPROPERTY(VisibleAnywhere, Category = "InventoryItem")
	int32 SlotIndex;
	UPROPERTY(VisibleAnywhere, Category = "InventoryItem")
	bool bHasItemData;
	UPROPERTY(VisibleAnywhere, Category = "InventoryItem")
	FInventoryItemData InventoryItemData;
	UPROPERTY(VisibleAnywhere, Category = "InventoryItem")
	TObjectPtr<UTexture2D> Icon;
	UPROPERTY(VisibleAnywhere, Category = "InventoryItem")
	int32 CurrentStackCount;

#pragma endregion

#pragma region Delegate

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemUsed);

	FOnItemUsed OnItemUsed;

#pragma endregion
};
