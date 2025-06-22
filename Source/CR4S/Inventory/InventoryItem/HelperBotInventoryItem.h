#pragma once

#include "CoreMinimal.h"
#include "BaseInventoryItem.h"
#include "FriendlyAI/Data/HelperBotSaveData.h"
#include "HelperBotInventoryItem.generated.h"

UCLASS()
class CR4S_API UHelperBotInventoryItem : public UBaseInventoryItem
{
	GENERATED_BODY()

#pragma region UBaseInventoryItem Override

public:
	virtual void UseItem(int32 Index) override;

#pragma endregion

#pragma region HelperBotData

public:
	FORCEINLINE const FHelperPickUpData& GetHelperBotData() const { return HelperBotData; }
	FORCEINLINE void SetHelperBotData(const FHelperPickUpData& NewHelperBotData) { HelperBotData = NewHelperBotData; }

private:
	FHelperPickUpData HelperBotData = FHelperPickUpData();

#pragma endregion
};
