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
	virtual void NativePreConstruct() override;
	
	virtual bool IsItemAllowedByFilter(UBaseInventoryItem* Item) const override;
	
#pragma endregion

#pragma region PartsTypeTag

public:
	FORCEINLINE const FGameplayTag& GetPartsTypeTag() const { return PartsTypeTag; }

private:
	UPROPERTY(EditAnywhere, Category = "PartsTypeTag")
	FGameplayTag PartsTypeTag;

#pragma endregion

#pragma region BindWidget

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> BackgroundIcon;

	UPROPERTY(EditAnywhere, Category = "BackgroundIcon")
	TObjectPtr<UTexture2D> BackgroundIconTexture;
	
#pragma endregion 
};
