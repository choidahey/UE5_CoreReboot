#pragma once

#include "CoreMinimal.h"
#include "BaseInventoryComponent.h"
#include "Inventory/UI/InventoryContainerWidget.h"
#include "PlayerInventoryComponent.generated.h"


class UQuickSlotInventoryComponent;
class UInventoryContainerWidget;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UPlayerInventoryComponent : public UBaseInventoryComponent
{
	GENERATED_BODY()

#pragma region UBaseInventoryComponent Override

public:
	UPlayerInventoryComponent();

	virtual void BeginPlay() override;

	virtual FAddItemResult AddItem(FName RowName, int32 Count) override;

#pragma endregion

#pragma region InventoryContainerWidget

public:
	UFUNCTION(BlueprintCallable, Category = "PlayerInventoryComponent||InventoryContainerWidget")
	void OpenPlayerInventoryWidget(const int32 CraftingDifficulty = 0);
	void OpenOtherInventoryWidget(EInventoryType InventoryType, UBaseInventoryComponent* InventoryComponent) const;

	void CloseInventoryWidget() const;

private:
	UPROPERTY(EditDefaultsOnly)
	int32 InventoryContainerWidgetOrder;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInventoryContainerWidget> InventoryContainerWidgetClass;
	UPROPERTY()
	TObjectPtr<UInventoryContainerWidget> InventoryContainerWidgetInstance;
	
	UPROPERTY()
	TObjectPtr<ASurvivalHUD> SurvivalHUD;
	
#pragma endregion

#pragma region QuickSlotInventoryComponent

public:
	FORCEINLINE UBaseInventoryComponent* GetQuickSlotInventoryComponent() { return QuickSlotInventoryComponent; }
	
private:
	UPROPERTY()
	TObjectPtr<UBaseInventoryComponent> QuickSlotInventoryComponent;
	
#pragma endregion 
};
