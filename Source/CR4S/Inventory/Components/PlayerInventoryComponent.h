#pragma once

#include "CoreMinimal.h"
#include "BaseInventoryComponent.h"
#include "Gimmick/Components/InteractionComponent.h"
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
	void OpenPlayerInventoryWidget(const int32 CraftingDifficulty = 0) const;
	void OpenOtherInventoryWidget(EInventoryType InventoryType, UBaseInventoryComponent* InventoryComponent) const;

	void CloseInventoryWidget() const;

private:
	bool PrepareOpenInventory(UInteractionComponent* InteractionComponent = nullptr) const;
	
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
	UFUNCTION(BlueprintCallable)
	void UseItem(const int32 Index) const;

	FORCEINLINE UBaseInventoryComponent* GetQuickSlotInventoryComponent() { return QuickSlotInventoryComponent; }
	FORCEINLINE const FGameplayTag& GetHeldToolTag() const { return HeldToolTag; }
	FORCEINLINE void SetHeldToolTag(const FGameplayTag& NewHeldToolTags) { HeldToolTag = NewHeldToolTags; }

private:
	UPROPERTY()
	TObjectPtr<UBaseInventoryComponent> QuickSlotInventoryComponent;

	FGameplayTag HeldToolTag;

#pragma endregion
};
