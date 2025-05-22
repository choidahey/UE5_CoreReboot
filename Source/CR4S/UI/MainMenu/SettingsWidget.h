#pragma once

#include "Blueprint/UserWidget.h"
#include "SettingsWidget.generated.h"

class UButton;
class UBaseWindowWidget;

UCLASS()
class CR4S_API USettingsWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	void HandleCloseWindow();
	UFUNCTION()
	void HandleOpenWindow();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UBaseWindowWidget* WindowWidget;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeIn;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeOut;
};
