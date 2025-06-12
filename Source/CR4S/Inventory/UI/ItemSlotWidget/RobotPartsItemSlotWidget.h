#pragma once

#include "CoreMinimal.h"
#include "BaseItemSlotWidget.h"
#include "GameplayTagContainer.h"
#include "RobotPartsItemSlotWidget.generated.h"

UCLASS()
class CR4S_API URobotPartsItemSlotWidget : public UBaseItemSlotWidget
{
	GENERATED_BODY()

#pragma region UBaseItemSlotWidget

public:
	virtual bool IsItemAllowedByFilter(UBaseInventoryItem* Item) const override;
	
#pragma endregion

#pragma region PartsTypeTag

public:
	FORCEINLINE const FGameplayTag& GetPartsTypeTag() const { return PartsTypeTag; }

private:
	UPROPERTY(EditAnywhere, Category = "PartsTypeTag")
	FGameplayTag PartsTypeTag;

#pragma endregion
};
