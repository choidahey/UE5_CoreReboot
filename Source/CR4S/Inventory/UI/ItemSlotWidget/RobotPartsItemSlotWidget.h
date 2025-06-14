#pragma once

#include "CoreMinimal.h"
#include "BaseItemSlotWidget.h"
#include "GameplayTagContainer.h"
#include "Inventory/UI/InventoryWidget/RobotInventoryWidget.h"
#include "RobotPartsItemSlotWidget.generated.h"

class URobotInventoryWidget;

UCLASS()
class CR4S_API URobotPartsItemSlotWidget : public UBaseItemSlotWidget
{
	GENERATED_BODY()

#pragma region UBaseItemSlotWidget

public:
	virtual void NativePreConstruct() override;

	virtual void InitSlotWidgetData(UBaseInventoryWidget* NewInventoryWidget, UBaseInventoryItem* NewItem) override;
	
	virtual bool IsItemAllowedByFilter(UBaseInventoryItem* Item) const override;

private:
	UPROPERTY()
	TObjectPtr<URobotInventoryWidget> RobotInventoryWidget;
	
#pragma endregion

#pragma region PartsTypeTag

public:
	FORCEINLINE const FGameplayTag& GetPartsTypeTag() const { return PartsTypeTag; }

private:
	UPROPERTY(EditAnywhere, Category = "RobotPartsItemSlotWidget|PartsTypeTag")
	FGameplayTag PartsTypeTag;
	UPROPERTY(EditAnywhere, Category = "RobotPartsItemSlotWidget|PartsTypeName")
	FText PartsTypeName;
	
#pragma endregion

#pragma region CreateMode

public:
	UFUNCTION()
	void OpenRecipeSelectWidget();
	UFUNCTION()
	void UpdateRobotWorkshopMode(ERobotWorkshopMode RobotWorkshopMode);

	UPROPERTY(EditAnywhere, Category = "RobotPartsItemSlotWidget|CreateMode")
	bool bCanCreateParts;
	
#pragma endregion 

#pragma region BindWidget

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> BackgroundIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SelectButton;

	UPROPERTY(EditAnywhere, Category = "BackgroundIcon")
	FSlateBrush BackgroundIconBrush;
	
	
#pragma endregion 
};
