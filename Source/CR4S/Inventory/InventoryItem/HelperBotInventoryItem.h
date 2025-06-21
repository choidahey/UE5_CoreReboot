#pragma once

#include "CoreMinimal.h"
#include "BaseInventoryItem.h"
#include "HelperBotInventoryItem.generated.h"

UCLASS()
class CR4S_API UHelperBotInventoryItem : public UBaseInventoryItem
{
	GENERATED_BODY()

#pragma region UBaseInventoryItem Override

public:
	virtual void UseItem(int32 Index) override;
	
#pragma endregion
};
