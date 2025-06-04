#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CraftingCategoryButtonWidget.generated.h"

class UCraftingContainerWidget;

UCLASS()
class CR4S_API UCraftingCategoryButtonWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region Initialize

public:
	void InitWidget(UCraftingContainerWidget* CraftingContainerWidget);
	
#pragma endregion 
	
#pragma region BindWidget

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Button;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Icon;
	
	UPROPERTY(EditAnywhere, Category = "Icon")
	UTexture2D* IconTexture;
	
#pragma endregion 
};
