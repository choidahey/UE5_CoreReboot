#pragma once

#include "CoreMinimal.h"
#include "BaseInventoryItem.h"

#include "ToolInventoryItem.generated.h"

UCLASS()
class CR4S_API UToolInventoryItem : public UBaseInventoryItem
{
	GENERATED_BODY()

public:
	virtual void UseItem() override;

	virtual void SetInventoryItemData(const FInventoryItemData& NewInventoryItemData, const int32 StackCount = 0) override;
	
private:
	FToolItemData ToolItemData;
};
