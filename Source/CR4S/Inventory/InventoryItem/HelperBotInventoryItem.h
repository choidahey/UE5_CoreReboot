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
	virtual void InitInventoryItem(UBaseInventoryComponent* NewInventoryComponent, const FInventoryItemData& NewInventoryItemData, const int32 StackCount = 0) override;
	virtual void UseItem(int32 Index) override;

#pragma endregion

#pragma region HelperBotData

public:
	FORCEINLINE const FHelperPickUpData& GetHelperBotData() const { return HelperBotData; }
	void SetHelperBotData(const FHelperPickUpData& NewHelperBotData);

private:
	FHelperPickUpData HelperBotData;

	FText BotNameText;
	FText HPText;

#pragma endregion

#pragma region Save & Load

public:
	virtual FInventoryItemSaveGame GetInventoryItemSaveData() override;
	virtual void LoadInventoryItemSaveData(UBaseInventoryComponent* NewInventoryComponent, const FInventoryItemSaveGame& ItemSaveGame) override;
	
#pragma endregion
};
