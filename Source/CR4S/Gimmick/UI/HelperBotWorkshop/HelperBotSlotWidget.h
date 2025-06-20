#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HelperBotSlotWidget.generated.h"

class UBorder;
class UButton;
class UHelperBotManagementWidget;

UCLASS()
class CR4S_API UHelperBotSlotWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region Initialize

public:
	void InitWidget(UHelperBotManagementWidget* HelperBotManagementWidget);
	
#pragma endregion

#pragma region DragAndDrop

public:
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
#pragma endregion

#pragma region ChangeOutLineColor

private:
	
	
#pragma endregion 

#pragma region BindWidget

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> OutLineBorder;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> HelperBotManagementButton;

	FLinearColor DefaultOutLineBorderColor;
	
#pragma endregion 
};
