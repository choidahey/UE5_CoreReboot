#pragma once

#include "Blueprint/UserWidget.h"
#include "SettingsWidget.generated.h"

class UButton;
class UBorder;
class UVerticalBox;
class USlider;
class UBaseWindowWidget;
class UMainMenuWidget;
class UAudioManager;

UCLASS()
class CR4S_API USettingsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void HandleCloseWindow();
	UFUNCTION()
	void HandleOpenWindow();

	UPROPERTY()
	TObjectPtr<UMainMenuWidget> MainMenuWidgetRef;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;


#pragma region Widget Bindings

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBaseWindowWidget> WindowWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> GameplayButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> AudioButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> GraphicsButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ControlsButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> GameplaySettings;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> AudioSettings;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> GraphicsSettings;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> ControlsSettings;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> MasterVolumeSlider;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> BGMVolumeSlider;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> SFXVolumeSlider;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> ParamZone;

#pragma endregion

#pragma region Animations
	void SetupButtonAnimation(UButton* Button);
	UButton* GetHoveredButton() const;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> FadeIn;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> FadeOut;

	TMap<TObjectPtr<UButton>, FVector2D> TargetOffsets;
	TSet<TObjectPtr<UButton>> HoveredButtons;

#pragma endregion

#pragma region Handlers

protected:
	UFUNCTION()
	void HandleGamePlayButtonClicked();
	UFUNCTION()
	void HandleAudioButtonClicked();
	UFUNCTION()
	void HandleGraphicsButtonClicked();
	UFUNCTION()
	void HandleControlsButtonClicked();
	UFUNCTION()
	void HandleButtonHovered();
	UFUNCTION()
	void HandleButtonUnhovered();
	UFUNCTION()
	void OnMasterVolumeChanged(float Value);
	UFUNCTION()
	void OnBGMVolumeChanged(float Value);
	UFUNCTION()
	void OnSFXVolumeChanged(float Value);

private:

	void HandleOpenCategory(TObjectPtr<UBorder> SettingsCategory);
	bool IsCategoryValid();

	TObjectPtr<UBorder> CurrentCategory = nullptr;

#pragma endregion


#pragma region Audio Settings

protected:
	void InitializeAudioSettings();

private:
	TObjectPtr<UAudioManager> AudioManager;


#pragma endregion
};
