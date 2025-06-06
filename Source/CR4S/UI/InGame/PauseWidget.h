#pragma once

#include "Blueprint/UserWidget.h"
#include "PauseWidget.generated.h"

class UButtonWidget;
class USettingsWidget;

DECLARE_DELEGATE(FOnResumeRequested);

UCLASS()
class CR4S_API UPauseWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnResumeRequested OnResumeRequested;

protected:
	UFUNCTION()
	void OnResumeButtonClicked();
	UFUNCTION()
	void OnSettingsButtonClicked();
	UFUNCTION()
	void OnToMenuButtonClicked();

	UPROPERTY(meta = (BindWidget))
	UButtonWidget* ResumeButton;
	UPROPERTY(meta = (BindWidget))
	UButtonWidget* SettingsButton;
	UPROPERTY(meta = (BindWidget))
	UButtonWidget* ToMenuButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<USettingsWidget> SettingsWidgetClass;

	UPROPERTY()
	USettingsWidget* SettingsWidgetInstance;

	virtual void NativeConstruct() override;
};
