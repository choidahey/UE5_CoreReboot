#pragma once

#include "CoreMinimal.h"
#include "BaseInventoryComponent.h"
#include "Inventory/UI/InventoryContainerWidget.h"
#include "PlayerInventoryComponent.generated.h"


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
	void OpenPlayerInventoryWidget();
	void OpenOtherInventoryWidget(EInventoryType InventoryType, UBaseInventoryComponent* InventoryComponent);

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

#pragma region QuickSlot

public:
	UBaseInventoryItem* GetQuickSlotItemDataByIndex(const int32 Index) const;
	
private:
	UPROPERTY(VisibleAnywhere, Category = "QuickSlot")
	TArray<TObjectPtr<UBaseInventoryItem>> QuickSlotItems;

	int32 QuickSlotCount;
	
#pragma endregion 

#pragma region QuickSlotWidget

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuickSlotItemChanged, UBaseInventoryItem*, Item);
	FOnQuickSlotItemChanged OnQuickSlotItemChanged;

private:
	void NotifyQuickSlotItemChanged(const int32 ItemSlotIndex) const;
	void NotifyQuickSlotItemsChanged(const TArray<int32>& ChangedItemSlots) const;
	
#pragma endregion
};
