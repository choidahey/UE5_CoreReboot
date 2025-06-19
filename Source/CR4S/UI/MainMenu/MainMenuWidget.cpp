#include "UI/MainMenu/MainMenuWidget.h"
#include "UI/MainMenu/SettingsWidget.h"
#include "UI/MainMenu/CreditsWidget.h"
#include "UI/MainMenu/DifficultyOptionsWidget.h"
#include "UI/MainMenu/GameSaveWidget.h"
#include "UI/Common/ConfirmWidget.h"
#include "UI/Common/ButtonWidget.h"
#include "Components/TextBlock.h"
#include "Game/System/AudioManager.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CreateChildWidgets();

	if (PlayGameButton)
	{
		PlayGameButton->OnClicked().AddDynamic(this, &UMainMenuWidget::OnPlayGameButtonClicked);
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

	if (UAudioManager* AudioManager = GetGameInstance()->GetSubsystem<UAudioManager>())
	{
		AudioManager->PlayBGM(MainMenuBGM);
	}

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
	if(!GameSaveWidgetInstance && GameSaveWidgetClass)
	{
		GameSaveWidgetInstance = CreateWidget<UGameSaveWidget>(GetWorld(), GameSaveWidgetClass);
	}
	if (!ConfirmWidgetInstance && ConfirmWidgetClass)
	{
		ConfirmWidgetInstance = CreateWidget<UConfirmWidget>(GetWorld(), ConfirmWidgetClass);
	}
}

void UMainMenuWidget::OnPlayGameButtonClicked()
{
	if (GameSaveWidgetInstance)
	{
		if (!GameSaveWidgetInstance->IsInViewport())
		{
			GameSaveWidgetInstance->AddToViewport();
		}

		//HideMenuButtons();
		GameSaveWidgetInstance->ParentWidgetRef = this;
		GameSaveWidgetInstance->HandleOpenWindow();
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

void UMainMenuWidget::OnQuitButtonClicked()
{
	if (GEngine)
	{
		GEngine->GameViewport->GetWindow()->RequestDestroyWindow();
	}
}