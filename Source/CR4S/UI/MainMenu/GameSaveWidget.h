#pragma once

#include "Blueprint/UserWidget.h"
#include "GameSaveWidget.generated.h"

class UButtonWidget;

UCLASS()
class CR4S_API UGameSaveWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY()
	TObjectPtr<UUserWidget> ParentWidgetRef;

	UFUNCTION()
	void HandleOpenWindow();
	UFUNCTION()
	void HandleCloseWindow();
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButtonWidget> BackButton;

};
