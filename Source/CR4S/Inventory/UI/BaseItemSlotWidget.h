#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseItemSlotWidget.generated.h"

class UBorder;
class UTextBlock;
class UImage;
class UInventoryComponent;
class UBaseInventoryItem;

UCLASS()
class CR4S_API UBaseItemSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
#pragma region Initalize

public:
	void InitWidget(UInventoryComponent* InInventorySystemComponent, UBaseInventoryItem* NewItem);

protected:
	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventorySystemComponent;
	
#pragma endregion
	
#pragma region BindWidget

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> IconImage;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CountTextBlock;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> CountTextBorder;
	
#pragma endregion

#pragma region Item

public:
	void SetItem(UBaseInventoryItem* InItem);

protected:
	UPROPERTY()
	TObjectPtr<UBaseInventoryItem> CurrentItem;
	
#pragma endregion
};
