#pragma once

#include "CoreMinimal.h"
#include "BaseInventoryItem.h"

#include "ToolInventoryItem.generated.h"

UCLASS()
class CR4S_API UToolInventoryItem : public UBaseInventoryItem
{
	GENERATED_BODY()

#pragma region UBaseInventoryItem Override
	
public:
	virtual void UseItem() override;

	virtual void SetInventoryItemData(const FInventoryItemData& NewInventoryItemData, const int32 StackCount = 0) override;

#pragma endregion

#pragma region Data
public:
	FORCEINLINE const FToolItemData& GetToolItemData() const { return ToolItemData; }
	
private:
	FToolItemData ToolItemData;

#pragma endregion
};
