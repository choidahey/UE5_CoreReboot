#pragma once

#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UButton;
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
	

public:
	UFUNCTION()
	void FadeOutBGM(float FadeDuration = 1.0f);

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
	void ShowGameButtons();
	void HideGameButtons();

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
	UButton* PlayGameButton;
	UPROPERTY(meta = (BindWidget))
	UButton* SettingsButton;
	UPROPERTY(meta = (BindWidget))
	UButton* CreditsButton;
	UPROPERTY(meta = (BindWidget))
	UButton* QuitButton;
	UPROPERTY(meta = (BindWidget))
	UButton* NewGameButton;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* NewGameText;
	UPROPERTY(meta = (BindWidget))
	UButton* LoadGameButton;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* LoadGameText;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeIn;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeOut;

	FTimerHandle FadeOutTimerHandle;
};
