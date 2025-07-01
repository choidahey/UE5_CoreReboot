#pragma once

#include "Blueprint/UserWidget.h"
#include "LoadingWidget.generated.h"

class UBorder;

UCLASS()
class CR4S_API ULoadingWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void HideImage();

protected:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> LoadingBorder;
};
