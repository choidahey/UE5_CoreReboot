#include "UI/MainMenu/SettingsWidget.h"
#include "UI/Common/BaseWindowWidget.h"
#include "UI/MainMenu/MainMenuWidget.h"
#include "UI/Common/NotificationWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/VerticalBox.h"
#include "Components/Slider.h"
#include "Components/CanvasPanelSlot.h"
#include "Game/System/AudioManager.h"
#include "Game/SaveGame/SaveGameManager.h"
#include "Game/SaveGame/SettingsSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameUserSettings.h"
#include "Engine/Engine.h"

void USettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!WindowWidget->OnBackClicked.IsAlreadyBound(this, &USettingsWidget::HandleCloseWindow))
	{
		WindowWidget->OnBackClicked.AddDynamic(this, &USettingsWidget::HandleCloseWindow);
	}

	if (!WindowWidget->OnApplyClicked.IsAlreadyBound(this, &USettingsWidget::RequestSaveSettings))
	{
		WindowWidget->OnApplyClicked.AddDynamic(this, &USettingsWidget::RequestSaveSettings);
	}



	if (IsCategoryValid())
	{
		GameplayButton->OnClicked.AddDynamic(this, &USettingsWidget::HandleGamePlayButtonClicked);
		AudioButton->OnClicked.AddDynamic(this, &USettingsWidget::HandleAudioButtonClicked);
		GraphicsButton->OnClicked.AddDynamic(this, &USettingsWidget::HandleGraphicsButtonClicked);
		ControlsButton->OnClicked.AddDynamic(this, &USettingsWidget::HandleControlsButtonClicked);
	}

	if (WindowModeLeftButton)
	{
		WindowModeLeftButton->OnClicked.AddDynamic(this, &USettingsWidget::OnWindowModeLeftClicked);
	}
	if (WindowModeRightButton)
	{
		WindowModeRightButton->OnClicked.AddDynamic(this, &USettingsWidget::OnWindowModeRightClicked);
	}
	if (ResolutionLeftButton)
	{
		ResolutionLeftButton->OnClicked.AddDynamic(this, &USettingsWidget::OnResolutionLeftClicked);
	}
	if (ResolutionRightButton)
	{
		ResolutionRightButton->OnClicked.AddDynamic(this, &USettingsWidget::OnResolutionRightClicked);
	}
	if (QualityLeftButton)
	{
		QualityLeftButton->OnClicked.AddDynamic(this, &USettingsWidget::OnGraphicsQualityLeftClicked);
	}
	if (QualityRightButton)
	{
		QualityRightButton->OnClicked.AddDynamic(this, &USettingsWidget::OnGraphicsQualityRightClicked);
	}

	ResolutionOptions = {
	FIntPoint(1280, 720),
	FIntPoint(1600, 900),
	FIntPoint(1920, 1080),
	FIntPoint(2560, 1440),
	FIntPoint(3840, 2160)
	};

	FIntPoint CurrentResolution = GEngine->GetGameUserSettings()->GetScreenResolution();
	for (int32 i = 0; i < ResolutionOptions.Num(); ++i)
	{
		if (ResolutionOptions[i] == CurrentResolution)
		{
			CurrentResolutionIndex = i;
			break;
		}
	}

	UpdateResolutionText();

	MasterVolumeSlider->OnValueChanged.AddDynamic(this, &USettingsWidget::OnMasterVolumeChanged);
	BGMVolumeSlider->OnValueChanged.AddDynamic(this, &USettingsWidget::OnBGMVolumeChanged);
	SFXVolumeSlider->OnValueChanged.AddDynamic(this, &USettingsWidget::OnSFXVolumeChanged);

	SetupButtonAnimation(GameplayButton);
	SetupButtonAnimation(AudioButton);
	SetupButtonAnimation(GraphicsButton);
	SetupButtonAnimation(ControlsButton);

	LoadSettingsData();
}

void USettingsWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	for (auto& Elem : TargetOffsets)
	{
		if (!Elem.Key) continue;

		FVector2D Current = Elem.Key->RenderTransform.Translation;
		FVector2D Target = Elem.Value;
		FVector2D New = FMath::Vector2DInterpTo(Current, Target, InDeltaTime, 10.f);
		Elem.Key->SetRenderTranslation(New);
	}
}

void USettingsWidget::SetupButtonAnimation(UButton* Button)
{
	if (!Button) return;

	TargetOffsets.Add(Button, FVector2D::ZeroVector);

	Button->OnHovered.AddDynamic(this, &USettingsWidget::HandleButtonHovered);
	Button->OnUnhovered.AddDynamic(this, &USettingsWidget::HandleButtonUnhovered);
}

void USettingsWidget::HandleButtonHovered()
{
	for (const auto& Pair : TargetOffsets)
	{
		if (Pair.Key && Pair.Key->IsHovered())
		{
			TargetOffsets[Pair.Key] = FVector2D(10.f, 0.f);
			break;
		}
	}
}

void USettingsWidget::HandleButtonUnhovered()
{
	for (const auto& Pair : TargetOffsets)
	{
		TargetOffsets[Pair.Key] = FVector2D(0.f, 0.f);
	}
}

UButton* USettingsWidget::GetHoveredButton() const
{
	for (const auto& Pair : TargetOffsets)
	{
		if (Pair.Key && Pair.Key->IsHovered())
		{
			return Pair.Key;
		}
	}
	return nullptr;
}

void USettingsWidget::HandleOpenWindow()
{
	SetVisibility(ESlateVisibility::Visible);
	PlayAnimation(FadeIn);
}

void USettingsWidget::HandleCloseWindow()
{
	PlayAnimation(FadeOut);

	FTimerHandle HideTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		HideTimerHandle,
		[this]()
		{
			SetVisibility(ESlateVisibility::Collapsed);
			if (MainMenuWidgetRef)
			{
				MainMenuWidgetRef->ShowMenuButtons();
			}
		},
		0.3f,
		false
	);
}

void USettingsWidget::HandleGamePlayButtonClicked()
{
	HandleOpenCategory(GameplaySettings);
}

void USettingsWidget::HandleAudioButtonClicked()
{
	HandleOpenCategory(AudioSettings);
}

void USettingsWidget::HandleGraphicsButtonClicked()
{
	HandleOpenCategory(GraphicsSettings);
}

void USettingsWidget::HandleControlsButtonClicked()
{
	HandleOpenCategory(ControlsSettings);
}


void USettingsWidget::HandleOpenCategory(TObjectPtr<UBorder> SettingsCategory)
{	
	if (CurrentCategory && CurrentCategory != SettingsCategory)
	{
		ParamZone->RemoveChild(CurrentCategory);
		CurrentCategory->SetVisibility(ESlateVisibility::Collapsed);
	}

	ParamZone->AddChild(SettingsCategory);
	SettingsCategory->SetVisibility(ESlateVisibility::Visible);
	CurrentCategory = SettingsCategory;
}

bool USettingsWidget::IsCategoryValid()
{
	return GameplaySettings && GameplayButton
		&& AudioSettings && AudioButton
		&& GraphicsSettings && GraphicsButton
		&& ControlsSettings && ControlsButton;
}

#pragma region "Graphics Settings"
void USettingsWidget::OnWindowModeLeftClicked()
{
	CurrentWindowModeIndex = (CurrentWindowModeIndex - 1 + WindowModeOptions.Num()) % WindowModeOptions.Num();
	UpdateWindowModeText();
	ApplyWindowModeSetting();
}

void USettingsWidget::OnWindowModeRightClicked()
{
	CurrentWindowModeIndex = (CurrentWindowModeIndex + 1) % WindowModeOptions.Num();
	UpdateWindowModeText();
	ApplyWindowModeSetting();
}

void USettingsWidget::UpdateWindowModeText()
{
	if (!WindowModeText) return;

	FString ModeString;
	switch (WindowModeOptions[CurrentWindowModeIndex])
	{
	case EWindowModeOption::Fullscreen:
		ModeString = TEXT("Full Screen");
		break;
	case EWindowModeOption::WindowedFullscreen:
		ModeString = TEXT("Windowed Fullscreen");
		break;
	case EWindowModeOption::Windowed:
		ModeString = TEXT("Windowed");
		break;
	}

	WindowModeText->SetText(FText::FromString(ModeString));
}

void USettingsWidget::ApplyWindowModeSetting()
{
	EWindowMode::Type NewMode;
	switch (WindowModeOptions[CurrentWindowModeIndex])
	{
	case EWindowModeOption::Fullscreen:
		NewMode = EWindowMode::Fullscreen;
		break;
	case EWindowModeOption::WindowedFullscreen:
		NewMode = EWindowMode::WindowedFullscreen;
		break;
	case EWindowModeOption::Windowed:
		NewMode = EWindowMode::Windowed;
		break;
	default:
		NewMode = EWindowMode::Windowed;
		break;
	}

	if (GEngine && GEngine->GetGameUserSettings())
	{
		UGameUserSettings* Settings = GEngine->GetGameUserSettings();
		Settings->SetFullscreenMode(NewMode);
		Settings->ApplySettings(false);
	}
}

void USettingsWidget::OnResolutionLeftClicked()
{
	CurrentResolutionIndex = (CurrentResolutionIndex - 1 + ResolutionOptions.Num()) % ResolutionOptions.Num();
	UpdateResolutionText();
	ApplyResolutionSetting();
}

void USettingsWidget::OnResolutionRightClicked()
{
	CurrentResolutionIndex = (CurrentResolutionIndex + 1) % ResolutionOptions.Num();
	UpdateResolutionText();
	ApplyResolutionSetting();
}

void USettingsWidget::UpdateResolutionText()
{
	if (!ResolutionText) return;

	FIntPoint Res = ResolutionOptions[CurrentResolutionIndex];
	FString ResStr = FString::Printf(TEXT("%d X %d"), Res.X, Res.Y);
	ResolutionText->SetText(FText::FromString(ResStr));
}

void USettingsWidget::ApplyResolutionSetting()
{
	if (GEngine && GEngine->GetGameUserSettings())
	{
		UGameUserSettings* Settings = GEngine->GetGameUserSettings();
		Settings->SetScreenResolution(ResolutionOptions[CurrentResolutionIndex]);
		Settings->ApplySettings(false);
	}
}

void USettingsWidget::OnGraphicsQualityLeftClicked()
{
	CurrentGraphicsQualityIndex = (CurrentGraphicsQualityIndex - 1 + GraphicsQualityOptions.Num()) % GraphicsQualityOptions.Num();
	UpdateGraphicsQualityText();
	ApplyGraphicsQualitySetting();
}

void USettingsWidget::OnGraphicsQualityRightClicked()
{
	CurrentGraphicsQualityIndex = (CurrentGraphicsQualityIndex + 1) % GraphicsQualityOptions.Num();
	UpdateGraphicsQualityText();
	ApplyGraphicsQualitySetting();
}

void USettingsWidget::UpdateGraphicsQualityText()
{
	if (!QualityText) return;

	FString QualityStr;
	switch (GraphicsQualityOptions[CurrentGraphicsQualityIndex])
	{
	case EGraphicsQualityOption::Epic: QualityStr = TEXT("Epic"); break;
	case EGraphicsQualityOption::High: QualityStr = TEXT("High"); break;
	case EGraphicsQualityOption::Medium: QualityStr = TEXT("Medium"); break;
	case EGraphicsQualityOption::Low: QualityStr = TEXT("Low"); break;
	}
	QualityText->SetText(FText::FromString(QualityStr));
}

void USettingsWidget::ApplyGraphicsQualitySetting()
{
	if (!GEngine || !GEngine->GetGameUserSettings()) return;

	UGameUserSettings* Settings = GEngine->GetGameUserSettings();

	int32 QualityLevel = 3; // Epic
	switch (GraphicsQualityOptions[CurrentGraphicsQualityIndex])
	{
	case EGraphicsQualityOption::Epic: QualityLevel = 3; break;
	case EGraphicsQualityOption::High: QualityLevel = 2; break;
	case EGraphicsQualityOption::Medium: QualityLevel = 1; break;
	case EGraphicsQualityOption::Low: QualityLevel = 0; break;
	}

	// 전체 항목 품질 적용
	Settings->SetOverallScalabilityLevel(QualityLevel);
	Settings->ApplySettings(false);
}

#pragma endregion

#pragma region "Audio Settings"

void USettingsWidget::OnMasterVolumeChanged(float Value)
{
	if (AudioManager)
	{
		AudioManager->SetMasterVolume(Value);
	}
}

void USettingsWidget::OnBGMVolumeChanged(float Value)
{
	if (AudioManager)
	{
		AudioManager->SetBGMVolume(Value);
	}
}

void USettingsWidget::OnSFXVolumeChanged(float Value)
{
	if (AudioManager)
	{
		AudioManager->SetSFXVolume(Value);
	}
}

#pragma endregion

void USettingsWidget::RequestSaveSettings()
{
	USaveGameManager* SaveManager = GetGameInstance()->GetSubsystem<USaveGameManager>();
	if (SaveManager)
	{
		SaveManager->SaveSettings();

		if (NotificationWidgetClass)
		{
			UNotificationWidget* Notification = CreateWidget<UNotificationWidget>(GetWorld(), NotificationWidgetClass);
			if (Notification)
			{
				Notification->AddToViewport(100);
				Notification->ShowNotification(FText::FromString(TEXT("Settings saved.")), 1.5f);
			}
		}
	}

	if (GEngine && GEngine->GetGameUserSettings())
	{
		UGameUserSettings* Settings = GEngine->GetGameUserSettings();
		Settings->SaveSettings();
	}
	// index 저장
}

void USettingsWidget::LoadSettingsData()
{
	AudioManager = GetGameInstance()->GetSubsystem<UAudioManager>();

	USaveGameManager* SaveManager = GetGameInstance()->GetSubsystem<USaveGameManager>();
	if (!SaveManager) return;

	USettingsSaveGame* SettingsData = SaveManager->LoadSettings();
	if (!SettingsData) return;

	if (MasterVolumeSlider)
	{
		MasterVolumeSlider->SetValue(SettingsData->MasterVolume);
	}
	if (BGMVolumeSlider)
	{
		BGMVolumeSlider->SetValue(SettingsData->BGMVolume);
	}
	if (SFXVolumeSlider)
	{
		SFXVolumeSlider->SetValue(SettingsData->SFXVolume);
	}

	// TODO: ���, ��� �� �߰� UI ��� ����ȭ
}

