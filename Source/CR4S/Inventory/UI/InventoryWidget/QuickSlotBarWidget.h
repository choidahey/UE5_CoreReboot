#pragma once

#include "CoreMinimal.h"
#include "BaseInventoryWidget.h"
#include "Blueprint/UserWidget.h"
#include "QuickSlotBarWidget.generated.h"

class UPlayerInventoryComponent;
class UBaseInventoryItem;

UCLASS()
class CR4S_API UQuickSlotBarWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region UUserWidget Override

public:
	virtual void NativeConstruct() override;
	
#pragma endregion

#pragma region Initalize

public:
	void InitWidget(UPlayerInventoryComponent* InPlayerInventoryComponent);

private:
	UPROPERTY()
	TObjectPtr<UPlayerInventoryComponent> PlayerInventoryComponent;
	
#pragma endregion
	
#pragma region BindWidget

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UHorizontalBox> QuickSlotBar;

	UPROPERTY()
	TArray<TObjectPtr<class UBaseItemSlotWidget>> QuickSlotWidgets;
	
#pragma endregion

#pragma region QuickSlotWidget

public:
	UFUNCTION()
	void UpdateQuickSlotWidget(UBaseInventoryItem* Item);
	
#pragma endregion
};
