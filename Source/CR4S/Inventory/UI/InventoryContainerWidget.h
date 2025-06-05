#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/Components/BaseInventoryComponent.h"
#include "ItemSlotWidget/BaseItemSlotWidget.h"
#include "InventoryContainerWidget.generated.h"

class UCraftingContainerWidget;
class UCompostBinWidget;
class UStorageInventoryWidget;
enum class EInventoryType : uint8;
class UPlayerInventoryComponent;
class ASurvivalHUD;
class UBorder;
class UBaseInventoryWidget;
class UQuickSlotBarWidget;

UCLASS()
class CR4S_API UInventoryContainerWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region Initalize

public:
	virtual void NativeConstruct() override;

	void InitWidget(ASurvivalHUD* InSurvivalHUD, UPlayerInventoryComponent* InPlayerInventoryComponent);

private:
	UPROPERTY()
	TObjectPtr<ASurvivalHUD> SurvivalHUD;
	UPROPERTY()
	TObjectPtr<UPlayerInventoryComponent> PlayerInventoryComponent;
	UPROPERTY()
	TObjectPtr<UBaseInventoryComponent> OtherInventoryComponent;
#pragma endregion

#pragma region ToggleWidget

public:
	void OpenPlayerInventoryWidget(bool bOpenCraftingWidget = false, int32 CraftingDifficulty = 0);
	void OpenOtherInventoryWidget(EInventoryType InventoryType, UBaseInventoryComponent* InventoryComponent);
	void OpenCraftingWidget(const int32 CraftingDifficulty);

	UFUNCTION()
	void CloseInventoryWidget();

	FORCEINLINE bool IsOpen() const { return bIsOpen; }

private:
	void InitToggleWidget(UUserWidget* Widget) const;
	UUserWidget* GetTargetInventoryWidget(EInventoryType InventoryType) const;

	bool bIsOpen;

#pragma endregion

#pragma region Input

public:
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	void MoveItemToInventory(const UBaseItemSlotWidget* ItemSlot, bool bTargetIsPlayer) const;

private:
	bool CanMoveItem(const bool bTargetIsPlayer) const;
	
#pragma endregion

#pragma region BindWidget
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> BackgroundBorder;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBaseInventoryWidget> PlayerInventoryWidget;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> InputGuideContainer;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBaseInventoryWidget> QuickSlotBarWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UStorageInventoryWidget> StorageInventoryWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBaseInventoryWidget> PlanterBoxInventoryWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCompostBinWidget> CompostBinWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCraftingContainerWidget> CraftingContainerWidget;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UUserWidget> OpenOtherWidget;

#pragma endregion
};
