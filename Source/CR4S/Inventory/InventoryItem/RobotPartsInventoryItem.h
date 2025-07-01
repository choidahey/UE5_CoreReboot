#pragma once

#include "CoreMinimal.h"
#include "BaseInventoryItem.h"
#include "RobotPartsInventoryItem.generated.h"

UCLASS()
class CR4S_API URobotPartsInventoryItem : public UBaseInventoryItem
{
	GENERATED_BODY()

#pragma region UBaseInventoryItem Override

public:
	virtual void InitInventoryItem(UBaseInventoryComponent* NewInventoryComponent,
	                               const FInventoryItemData& NewInventoryItemData,
	                               const int32 StackCount = 0) override;

#pragma endregion

#pragma region Data

public:
	void InitTextFormat();
private:
	void InitRobotPartsInfoData(const FGameplayTag& Tag);

	FTextFormat TextFormat;
	
#pragma endregion 
};
