#pragma once

#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UButtonWidget;
class UTextBlock;
class UConfirmWidget;
class USettingsWidget;
class UCreditsWidget;
class UDifficultyOptionsWidget;
class USoundBase;
class UAudioComponent;

UCLASS()
class CR4S_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnPlayGameButtonHovered();
	UFUNCTION()
	void OnPlayGameButtonUnhovered();
	UFUNCTION()
	void OnGameButtonHovered();
	UFUNCTION()
	void OnGameButtonUnhovered();
	UFUNCTION()
	void OnSettingsButtonClicked();
	UFUNCTION()
	void OnCreditsButtonClicked();
	UFUNCTION()
	void OnQuitButtonClicked();
	UFUNCTION()
	void OnNewGameButtonClicked();


	void CreateChildWidgets();

	void SetWidgetVisibility(UUserWidget* Widget, ESlateVisibility InVisibility);

public:
	void HideMenuButtons();
	void ShowMenuButtons();
	void ShowGameButtons();
	void HideGameButtons();

protected:
	UPROPERTY()
	UConfirmWidget* ConfirmWidgetInstance;
	UPROPERTY()
	UDifficultyOptionsWidget* DifficultyOptionsWidgetInstance;
	UPROPERTY()
	USettingsWidget* SettingsWidgetInstance;
	UPROPERTY()
	UCreditsWidget* CreditsWidgetInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UConfirmWidget> ConfirmWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UDifficultyOptionsWidget> DifficultyOptionsWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<USettingsWidget> SettingsWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UCreditsWidget> CreditsWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	USoundBase* MainMenuBGM;
	UPROPERTY()
	UAudioComponent* BGMComponent;

	UPROPERTY(meta = (BindWidget))
	UButtonWidget* PlayGameButton;
	UPROPERTY(meta = (BindWidget))
	UButtonWidget* SettingsButton;
	UPROPERTY(meta = (BindWidget))
	UButtonWidget* CreditsButton;
	UPROPERTY(meta = (BindWidget))
	UButtonWidget* QuitButton;
	UPROPERTY(meta = (BindWidget))
	UButtonWidget* NewGameButton;
	UPROPERTY(meta = (BindWidget))
	UButtonWidget* LoadGameButton;


	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeIn;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeOut;

	FTimerHandle FadeOutTimerHandle;
};
