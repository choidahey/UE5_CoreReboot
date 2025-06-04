#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CraftingCategorySelectWidget.generated.h"

class UCraftingContainerWidget;

UCLASS()
class CR4S_API UCraftingCategorySelectWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region Initialize

public:
	void InitWidget(UCraftingContainerWidget* CraftingContainerWidget);

#pragma endregion

#pragma region BindWidget

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> ButtonContainer;

	UPROPERTY()
	TArray<TObjectPtr<class UCraftingCategoryButtonWidget>> CraftingCategoryButtonWidgets;

#pragma endregion 
};
