#pragma once

#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UButtonWidget;
class UTextBlock;
class UImage;
class UGameIntroWidget;
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
	
#pragma region Constructors and Initializers

protected:
	UFUNCTION()
	void InitMainMenu();

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

#pragma endregion


#pragma region Widget Class and Assignables

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UGameIntroWidget> IntroWidgetClass;
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Background")
	TArray<UTexture2D*> BackgroundImages;

#pragma endregion

#pragma region Widget Instances

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

#pragma endregion

#pragma region Widget Bindings

protected:
	UPROPERTY(meta = (BindWidget))
	UButtonWidget* PlayGameButton;
	UPROPERTY(meta = (BindWidget))
	UButtonWidget* SettingsButton;
	UPROPERTY(meta = (BindWidget))
	UButtonWidget* CreditsButton;
	UPROPERTY(meta = (BindWidget))
	UButtonWidget* QuitButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> BackgroundImage;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* ShowButtonsAnim;

#pragma endregion

#pragma region Widget Controls
public:
	void PlayIntroSequence();

	void ShowBackground();

	UFUNCTION()
	void HideMenuButtons();
	UFUNCTION()
	void ShowMenuButtons();

protected:
	UFUNCTION(Category = "Background")
	void ShowNextBackground();

	UPROPERTY()
	UMaterialInstanceDynamic* BackgroundMID = nullptr;

private:
	void CreateChildWidgets();
	void SetWidgetVisibility(UUserWidget* Widget, ESlateVisibility InVisibility);

	int32 CurrentIndex = 0;

	FTimerHandle DissolveTimerHandle;


#pragma endregion

#pragma region Delegates & Events

protected:
	UFUNCTION()
	void OnPlayGameButtonClicked();
	UFUNCTION()
	void OnSettingsButtonClicked();
	UFUNCTION()
	void OnCreditsButtonClicked();
	UFUNCTION()
	void OnQuitButtonClicked();

public:

	FTimerHandle DissolveStepTimerHandle;
	FTimerHandle NextBackgroundTimerHandle;
};
