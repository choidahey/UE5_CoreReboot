#include "UI/MainMenu/MainMenuWidget.h"
#include "UI/MainMenu/SettingsWidget.h"
#include "UI/MainMenu/CreditsWidget.h"
#include "UI/MainMenu/DifficultyOptionsWidget.h"
#include "UI/Common/ConfirmWidget.h"
#include "UI/Common/ButtonWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CreateChildWidgets();

	if (PlayGameButton)
	{
		PlayGameButton->OnHovered().AddDynamic(this, &UMainMenuWidget::OnPlayGameButtonHovered);
		PlayGameButton->OnUnhovered().AddDynamic(this, &UMainMenuWidget::OnPlayGameButtonUnhovered);
	}
	if (NewGameButton)
	{
		NewGameButton->OnHovered().AddDynamic(this, &UMainMenuWidget::OnGameButtonHovered);
		NewGameButton->OnUnhovered().AddDynamic(this, &UMainMenuWidget::OnGameButtonUnhovered);
		NewGameButton->OnClicked().AddDynamic(this, &UMainMenuWidget::OnNewGameButtonClicked);
	}
	if (LoadGameButton)
	{
		LoadGameButton->OnHovered().AddDynamic(this, &UMainMenuWidget::OnGameButtonHovered);
		LoadGameButton->OnUnhovered().AddDynamic(this, &UMainMenuWidget::OnGameButtonUnhovered);
	}
	if (SettingsButton)
	{
		SettingsButton->OnClicked().AddDynamic(this, &UMainMenuWidget::OnSettingsButtonClicked);
	}
	if (CreditsButton)
	{
		//CreditsButton->OnClicked().AddDynamic(this, &UMainMenuWidget::OnCreditsButtonClicked);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked().AddDynamic(this, &UMainMenuWidget::OnQuitButtonClicked);
	}

	if (MainMenuBGM)
	{
		BGMComponent = UGameplayStatics::SpawnSound2D(this, MainMenuBGM, 1.0f, 1.0f, 0.0f, nullptr, true);
		if (BGMComponent)
		{
			BGMComponent->FadeIn(1.5f, 1.0f);
		}
	}

	HideGameButtons();
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
		if (!DifficultyOptionsWidgetInstance->IsInViewport())
		{
			DifficultyOptionsWidgetInstance->AddToViewport();
		}

		HideMenuButtons();
		DifficultyOptionsWidgetInstance->MainMenuWidgetRef = this;
		DifficultyOptionsWidgetInstance->HandleOpenWindow();
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

		HideMenuButtons();
		SettingsWidgetInstance->MainMenuWidgetRef = this;
		SettingsWidgetInstance->HandleOpenWindow();
	}
}

void UMainMenuWidget::OnCreditsButtonClicked()
{
	if (CreditsWidgetInstance)
	{
		CreditsWidgetInstance->AddToViewport(10);
		HideMenuButtons();
	}
}

void UMainMenuWidget::ShowGameButtons()
{
	SetWidgetVisibility(NewGameButton, ESlateVisibility::Visible);
	SetWidgetVisibility(LoadGameButton, ESlateVisibility::Visible);
}

void UMainMenuWidget::HideGameButtons()
{
	SetWidgetVisibility(NewGameButton, ESlateVisibility::Hidden);
	SetWidgetVisibility(LoadGameButton, ESlateVisibility::Hidden);
}

void UMainMenuWidget::ShowMenuButtons()
{
	SetWidgetVisibility(PlayGameButton, ESlateVisibility::Visible);
	SetWidgetVisibility(SettingsButton, ESlateVisibility::Visible);
	SetWidgetVisibility(CreditsButton, ESlateVisibility::Visible);
	SetWidgetVisibility(QuitButton, ESlateVisibility::Visible);
}

void UMainMenuWidget::HideMenuButtons()
{
	SetWidgetVisibility(PlayGameButton, ESlateVisibility::Hidden);
	SetWidgetVisibility(SettingsButton, ESlateVisibility::Hidden);
	SetWidgetVisibility(CreditsButton, ESlateVisibility::Hidden);
	SetWidgetVisibility(QuitButton, ESlateVisibility::Hidden);
}



void UMainMenuWidget::SetWidgetVisibility(UUserWidget* Widget, ESlateVisibility InVisibility)
{
	if (Widget)
	{
		Widget->SetVisibility(InVisibility);
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