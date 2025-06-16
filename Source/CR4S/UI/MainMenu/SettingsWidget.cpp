#include "UI/MainMenu/SettingsWidget.h"
#include "UI/Common/BaseWindowWidget.h"
#include "UI/MainMenu/MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/VerticalBox.h"
#include "Components/Slider.h"
#include "Game/System/AudioManager.h"
#include "Kismet/GameplayStatics.h"

void USettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!WindowWidget->OnBackClicked.IsAlreadyBound(this, &USettingsWidget::HandleCloseWindow))
	{
		WindowWidget->OnBackClicked.AddDynamic(this, &USettingsWidget::HandleCloseWindow);
	}

	if (IsCategoryValid())
	{
		GameplayButton->OnClicked.AddDynamic(this, &USettingsWidget::HandleGamePlayButtonClicked);
		AudioButton->OnClicked.AddDynamic(this, &USettingsWidget::HandleAudioButtonClicked);
		GraphicsButton->OnClicked.AddDynamic(this, &USettingsWidget::HandleGraphicsButtonClicked);
		ControlsButton->OnClicked.AddDynamic(this, &USettingsWidget::HandleControlsButtonClicked);
	}

	MasterVolumeSlider->OnValueChanged.AddDynamic(this, &USettingsWidget::OnMasterVolumeChanged);
	BGMVolumeSlider->OnValueChanged.AddDynamic(this, &USettingsWidget::OnBGMVolumeChanged);
	SFXVolumeSlider->OnValueChanged.AddDynamic(this, &USettingsWidget::OnSFXVolumeChanged);

	SetupButtonAnimation(GameplayButton);
	SetupButtonAnimation(AudioButton);
	SetupButtonAnimation(GraphicsButton);
	SetupButtonAnimation(ControlsButton);

	InitializeAudioSettings();
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

void USettingsWidget::InitializeAudioSettings()
{
	AudioManager = GetGameInstance()->GetSubsystem<UAudioManager>();
	if (AudioManager)
	{
		MasterVolumeSlider->SetValue(AudioManager->GetMasterVolume());
		BGMVolumeSlider->SetValue(AudioManager->GetBGMVolume());
		SFXVolumeSlider->SetValue(AudioManager->GetSFXVolume());
	}
}

#pragma endregion "Audio Settings"