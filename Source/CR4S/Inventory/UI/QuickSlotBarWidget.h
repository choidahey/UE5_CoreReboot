#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuickSlotBarWidget.generated.h"

class UBaseInventoryItem;
class UInventoryComponent;

UCLASS()
class CR4S_API UQuickSlotBarWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region UUserWidget Override

public:
	virtual bool Initialize() override;
	virtual void NativeConstruct() override;
	
#pragma endregion

#pragma region Initalize

public:
	void InitWidget(UInventoryComponent* InInventorySystemComponent);

private:
	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventorySystemComponent;
	
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
	void SetItemWidget(UBaseInventoryItem* Item);

private:
	UPROPERTY(EditDefaultsOnly)
	int32 MinQuickSlotIndex;
	UPROPERTY(EditDefaultsOnly)
	int32 MaxQuickSlotIndex;
	
#pragma endregion
};
