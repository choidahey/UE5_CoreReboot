#pragma once

#include "Blueprint/UserWidget.h"
#include "SettingsWidget.generated.h"

class UButton;
class UTextBlock;
class UBorder;
class UVerticalBox;
class USlider;
class UNotificationWidget;
class UBaseWindowWidget;
class UMainMenuWidget;
class UAudioManager;

UENUM()
enum class EWindowModeOption : uint8
{
	Fullscreen,
	WindowedFullscreen,
	Windowed,
};

UENUM()
enum class EGraphicsQualityOption : uint8
{
	Epic,
	High,
	Medium,
	Low,
};

UCLASS()
class CR4S_API USettingsWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region Constructors & Initiailizers

public:
	UPROPERTY()
	TObjectPtr<UMainMenuWidget> MainMenuWidgetRef;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	void InitGraphicsSettings();
	void InitAudioSettings();

#pragma endregion

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
	TObjectPtr<UButton> WindowModeLeftButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> WindowModeRightButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> WindowModeText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ResolutionLeftButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ResolutionRightButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ResolutionText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QualityLeftButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QualityRightButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> QualityText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> MasterVolumeSlider;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> BGMVolumeSlider;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> SFXVolumeSlider;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> ParamZone;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UNotificationWidget> NotificationWidgetClass;

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
public:
	UFUNCTION()
	void HandleCloseWindow();
	UFUNCTION()
	void HandleOpenWindow();

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
	void OnWindowModeLeftClicked();
	UFUNCTION()
	void OnWindowModeRightClicked();

	UFUNCTION()
	void OnResolutionLeftClicked();
	UFUNCTION()
	void OnResolutionRightClicked();

	UFUNCTION()
	void OnGraphicsQualityLeftClicked();
	UFUNCTION()
	void OnGraphicsQualityRightClicked();

	UFUNCTION()
	void OnMasterVolumeChanged(float Value);
	UFUNCTION()
	void OnBGMVolumeChanged(float Value);
	UFUNCTION()
	void OnSFXVolumeChanged(float Value);

	UFUNCTION()
	void RequestSaveSettings();

private:

	void HandleOpenCategory(TObjectPtr<UBorder> SettingsCategory);
	bool IsCategoryValid();

	TObjectPtr<UBorder> CurrentCategory = nullptr;

#pragma endregion

#pragma region Graphics Settings


	void UpdateWindowModeText();
	void ApplyWindowModeSetting();
	
	void UpdateResolutionText();
	void ApplyResolutionSetting();

	void UpdateGraphicsQualityText();
	void ApplyGraphicsQualitySetting();

	static EWindowMode::Type ConvertToEWindowMode(EWindowModeOption Option);
	static int32 ConvertToQualityLevel(EGraphicsQualityOption Option);

	TArray<FIntPoint> ResolutionOptions = {
	FIntPoint(1280, 720),
	FIntPoint(1600, 900),
	FIntPoint(1920, 1080),
	FIntPoint(2560, 1440),
	FIntPoint(3840, 2160)
	};
	
	TArray<EWindowModeOption> WindowModeOptions = {
	EWindowModeOption::Fullscreen,
	EWindowModeOption::WindowedFullscreen,
	EWindowModeOption::Windowed
	};

	TArray<EGraphicsQualityOption> GraphicsQualityOptions = {
		EGraphicsQualityOption::Low,
		EGraphicsQualityOption::Medium,
		EGraphicsQualityOption::High,
		EGraphicsQualityOption::Epic
	};
	
	int32 CurrentWindowModeIndex = 0;
	int32 CurrentResolutionIndex = 2;
	int32 CurrentGraphicsQualityIndex = 1;

#pragma region Audio Settings
;

private:
	void LoadSettingsData();
	TObjectPtr<UAudioManager> AudioManager;
#pragma endregion
};
