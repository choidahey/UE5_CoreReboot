#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryContainerWidget.generated.h"

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
	void InitWidget(ASurvivalHUD* InSurvivalHUD, UPlayerInventoryComponent* InPlayerInventoryComponent);

private:
	UPROPERTY()
	TObjectPtr<ASurvivalHUD> SurvivalHUD;
	UPROPERTY()
	TObjectPtr<UPlayerInventoryComponent> PlayerInventoryComponent;
	
#pragma endregion

#pragma region ToggleWidget

public:
	void ToggleInventoryWidget(EInventoryType InventoryType);

private:
	bool bIsOpen;
	
#pragma endregion
	
#pragma region BindWidget

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> BackgroundBorder;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBaseInventoryWidget> PlayerInventoryWidget;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UQuickSlotBarWidget> QuickSlotBarWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBaseInventoryWidget> ItemPouchWidget;

	UPROPERTY(EditDefaultsOnly)
	TMap<EInventoryType, TObjectPtr<UBaseInventoryWidget>> InventoryWidgets;
	
#pragma endregion
};
