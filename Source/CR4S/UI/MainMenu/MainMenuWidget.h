#pragma once

#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UButtonWidget;
class UTextBlock;
class UConfirmWidget;
class USettingsWidget;
class UCreditsWidget;
class UDifficultyOptionsWidget;
class UGameSaveWidget;
class USoundBase;

UCLASS()
class CR4S_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnPlayGameButtonClicked();
	UFUNCTION()
	void OnSettingsButtonClicked();
	UFUNCTION()
	void OnCreditsButtonClicked();
	UFUNCTION()
	void OnQuitButtonClicked();

	void CreateChildWidgets();

	void SetWidgetVisibility(UUserWidget* Widget, ESlateVisibility InVisibility);

public:
	void HideMenuButtons();
	void ShowMenuButtons();

protected:
	UPROPERTY()
	TObjectPtr<UConfirmWidget> ConfirmWidgetInstance;
	UPROPERTY()
	TObjectPtr<UDifficultyOptionsWidget> DifficultyOptionsWidgetInstance;
	UPROPERTY()
	TObjectPtr<UGameSaveWidget> GameSaveWidgetInstance;
	UPROPERTY()
	TObjectPtr<USettingsWidget> SettingsWidgetInstance;
	UPROPERTY()
	TObjectPtr<UCreditsWidget> CreditsWidgetInstance;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UConfirmWidget> ConfirmWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UDifficultyOptionsWidget> DifficultyOptionsWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UGameSaveWidget> GameSaveWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<USettingsWidget> SettingsWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UCreditsWidget> CreditsWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	USoundBase* MainMenuBGM;

	UPROPERTY(meta = (BindWidget))
	UButtonWidget* PlayGameButton;
	UPROPERTY(meta = (BindWidget))
	UButtonWidget* SettingsButton;
	UPROPERTY(meta = (BindWidget))
	UButtonWidget* CreditsButton;
	UPROPERTY(meta = (BindWidget))
	UButtonWidget* QuitButton;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* ShowButtonsAnim;

	FTimerHandle FadeOutTimerHandle;
};
