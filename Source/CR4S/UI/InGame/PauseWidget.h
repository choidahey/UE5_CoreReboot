#pragma once

#include "Blueprint/UserWidget.h"
#include "PauseWidget.generated.h"

class UButtonWidget;
class USettingsWidget;
class UHelpWidget;
class UOverlay;

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
	void OnHelpButtonClicked();
	UFUNCTION()
	void OnToMenuButtonClicked();

	UFUNCTION()
	void HideMenu();
	UFUNCTION()
	void ShowMenu();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButtonWidget> ResumeButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButtonWidget> SettingsButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButtonWidget> HelpButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButtonWidget> ToMenuButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> PauseMenuOverlay;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> HideMenuAnim;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> ShowMenuAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<USettingsWidget> SettingsWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UHelpWidget> HelpWidgetClass;

	UPROPERTY()
	USettingsWidget* SettingsWidgetInstance;
	UPROPERTY()
	TObjectPtr<UHelpWidget> HelpWidgetInstance;

	virtual void NativeConstruct() override;
};
