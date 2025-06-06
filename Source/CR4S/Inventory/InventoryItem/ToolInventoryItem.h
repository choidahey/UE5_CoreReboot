#pragma once

#include "CoreMinimal.h"
#include "BaseInventoryItem.h"

#include "ToolInventoryItem.generated.h"

class UPlayerCharacterStatusComponent;
class UPlayerInventoryComponent;

UCLASS()
class CR4S_API UToolInventoryItem : public UBaseInventoryItem
{
	GENERATED_BODY()

#pragma region UBaseInventoryItem Override
	
public:
	UToolInventoryItem();
	
	virtual void InitInventoryItem(UBaseInventoryComponent* NewInventoryComponent, const FInventoryItemData& NewInventoryItemData, const int32 StackCount = 0) override;

	virtual void UseItem(int32 Index) override;

#pragma endregion

#pragma region Equipment

public:
	void EquipItem() const;
	void UnEquipItem() const;
	
#pragma endregion 
	
#pragma region Data
public:
	FORCEINLINE const FToolItemData& GetToolItemData() const { return ToolItemData; }
	
private:
	FToolItemData ToolItemData;

	FGameplayTag DefaultTag;

#pragma endregion
};
