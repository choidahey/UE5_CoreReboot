#include "UI/MainMenu/MainMenuWidget.h"
#include "UI/MainMenu/SettingsWidget.h"
#include "UI/MainMenu/CreditsWidget.h"
#include "UI/MainMenu/DifficultyOptionsWidget.h"
#include "UI/Common/ConfirmWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CreateChildWidgets();

	if (PlayGameButton)
	{
		PlayGameButton->OnHovered.AddDynamic(this, &UMainMenuWidget::OnPlayGameButtonHovered);
		PlayGameButton->OnUnhovered.AddDynamic(this, &UMainMenuWidget::OnPlayGameButtonUnhovered);
	}
	if (NewGameButton)
	{
		NewGameButton->OnHovered.AddDynamic(this, &UMainMenuWidget::OnGameButtonHovered);
		NewGameButton->OnUnhovered.AddDynamic(this, &UMainMenuWidget::OnGameButtonUnhovered);
		NewGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnNewGameButtonClicked);
	}
	if (LoadGameButton)
	{
		LoadGameButton->OnHovered.AddDynamic(this, &UMainMenuWidget::OnGameButtonHovered);
		LoadGameButton->OnUnhovered.AddDynamic(this, &UMainMenuWidget::OnGameButtonUnhovered);
	}
	if (SettingsButton)
	{
		SettingsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnSettingsButtonClicked);
	}
	if (CreditsButton)
	{
		CreditsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnCreditsButtonClicked);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitButtonClicked);
	}

	if (MainMenuBGM)
	{
		BGMComponent = UGameplayStatics::SpawnSound2D(this, MainMenuBGM, 1.0f, 1.0f, 0.0f, nullptr, true);
		if (BGMComponent)
		{
			BGMComponent->FadeIn(1.5f, 1.0f);
		}
	}
}

void UMainMenuWidget::OnPlayGameButtonHovered()
{
	if (IsAnimationPlaying(FadeOut))
	{
		StopAnimation(FadeOut);
		GetWorld()->GetTimerManager().ClearTimer(FadeOutTimerHandle);
	}

	ShowGameButtons();
	PlayAnimation(FadeIn);
}

void UMainMenuWidget::OnPlayGameButtonUnhovered()
{
	if (IsAnimationPlaying(FadeIn))
	{
		StopAnimation(FadeIn);
	}

	PlayAnimation(FadeOut);

	if (GetWorld()->GetTimerManager().IsTimerActive(FadeOutTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(FadeOutTimerHandle);
	}

	GetWorld()->GetTimerManager().SetTimer(
		FadeOutTimerHandle,
		this,
		&UMainMenuWidget::HideGameButtons,
		0.6f,
		false
	);
}

void UMainMenuWidget::OnGameButtonHovered()
{
	if (IsAnimationPlaying(FadeOut))
	{
		StopAnimation(FadeOut);
		GetWorld()->GetTimerManager().ClearTimer(FadeOutTimerHandle);
	}

	ShowGameButtons();
}

void UMainMenuWidget::OnGameButtonUnhovered()
{
	if (IsAnimationPlaying(FadeIn))
	{
		StopAnimation(FadeIn);
	}

	PlayAnimation(FadeOut);
	if (GetWorld()->GetTimerManager().IsTimerActive(FadeOutTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(FadeOutTimerHandle);
	}
	GetWorld()->GetTimerManager().SetTimer(
		FadeOutTimerHandle,
		this,
		&UMainMenuWidget::HideGameButtons,
		0.6f,
		false
	);
}

void UMainMenuWidget::CreateChildWidgets()
{
	if (!SettingsWidgetInstance && SettingsWidgetClass)
	{
		SettingsWidgetInstance = CreateWidget<USettingsWidget>(GetWorld(), SettingsWidgetClass);
	}
	if (!CreditsWidgetInstance && CreditsWidgetClass)
	{
		CreditsWidgetInstance = CreateWidget<UCreditsWidget>(GetWorld(), CreditsWidgetClass);
	}
	if (!DifficultyOptionsWidgetInstance && DifficultyOptionsWidgetClass)
	{
		DifficultyOptionsWidgetInstance = CreateWidget<UDifficultyOptionsWidget>(GetWorld(), DifficultyOptionsWidgetClass);
	}
	if (!ConfirmWidgetInstance && ConfirmWidgetClass)
	{
		ConfirmWidgetInstance = CreateWidget<UConfirmWidget>(GetWorld(), ConfirmWidgetClass);
	}
}

void UMainMenuWidget::OnNewGameButtonClicked()
{
	if (DifficultyOptionsWidgetInstance)
	{
		DifficultyOptionsWidgetInstance->AddToViewport(10);

		DifficultyOptionsWidgetInstance->MainMenuWidgetRef = this;
	}
}

void UMainMenuWidget::OnSettingsButtonClicked()
{
	if (SettingsWidgetInstance)
	{
		if (!SettingsWidgetInstance->IsInViewport())
		{
			SettingsWidgetInstance->AddToViewport();
		}
		
		SettingsWidgetInstance->HandleOpenWindow();
	}
}

void UMainMenuWidget::OnCreditsButtonClicked()
{
	if (CreditsWidgetInstance)
	{
		CreditsWidgetInstance->AddToViewport(10);
	}
}

void UMainMenuWidget::ShowGameButtons()
{
	if (NewGameButton)
	{
		NewGameButton->SetVisibility(ESlateVisibility::Visible);
	}
	if (NewGameText)
	{
		NewGameText->SetVisibility(ESlateVisibility::Visible);
	}
	if (LoadGameButton)
	{
		LoadGameButton->SetVisibility(ESlateVisibility::Visible);
	}
	if (LoadGameText)
	{
		LoadGameText->SetVisibility(ESlateVisibility::Visible);
	}
}

void UMainMenuWidget::HideGameButtons()
{
	if (NewGameButton)
	{
		NewGameButton->SetVisibility(ESlateVisibility::Hidden);
	}
	if (NewGameText)
	{
		NewGameText->SetVisibility(ESlateVisibility::Hidden);
	}
	if (LoadGameButton)
	{
		LoadGameButton->SetVisibility(ESlateVisibility::Hidden);
	}
	if (LoadGameText)
	{
		LoadGameText->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMainMenuWidget::FadeOutBGM(float FadeDuration)
{
	if (BGMComponent && BGMComponent->IsPlaying())
	{
		BGMComponent->FadeOut(FadeDuration, 0.0f);
	}
}

void UMainMenuWidget::OnQuitButtonClicked()
{
	if (GEngine)
	{
		GEngine->GameViewport->GetWindow()->RequestDestroyWindow();
	}
}