#pragma once

#include "CoreMinimal.h"
#include "BaseItemSlotWidget.h"
#include "GameplayTagContainer.h"
#include "Inventory/UI/InventoryWidget/RobotInventoryWidget.h"
#include "RobotPartsItemSlotWidget.generated.h"

class URobotWeaponComponent;
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

	virtual void SetItem(UBaseInventoryItem* InItem) override;

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

#pragma region ModularRobot

public:
	FORCEINLINE void ResetModularRobotRef() { ModularRobot = nullptr; }

private:
	void EquipParts(const UBaseInventoryItem* Item) const;
	void UnEquipParts() const;

	bool EquippedBodyParts(const FGameplayTagContainer& ItemTags) const;

	UPROPERTY()
	TObjectPtr<AModularRobot> ModularRobot;

	UPROPERTY()
	TObjectPtr<URobotWeaponComponent> RobotWeaponComponent;

	UPROPERTY(EditAnywhere, Category = "RobotPartsItemSlotWidget|ModularRobot")
	int32 WeaponSlotIndex = -1;

#pragma endregion
};
