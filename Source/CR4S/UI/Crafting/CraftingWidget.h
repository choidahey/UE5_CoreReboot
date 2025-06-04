#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CraftingWidget.generated.h"

class UImage;

UCLASS()
class CR4S_API UCraftingWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region Initialize

public:
	void InitWidget();
	
#pragma endregion 
	
#pragma region ToggleWidget

public:
	void OpenWidget();
	void CloseWidget();
	
#pragma endregion

#pragma region BindWidget

private:
	// UPROPERTY(meta = (BindWidget))
	// TObjectPtr<UImage> 
	
#pragma endregion 
};
