#include "UI/InGame/PauseWidget.h"
#include "UI/InGame/HelpWidget.h"
#include "UI/Common/ButtonWidget.h"
#include "UI/MainMenu/SettingsWidget.h"
#include "Components/Overlay.h"
#include "Game/GameMode/C4SurvivalGameMode.h"
#include "Kismet/GameplayStatics.h"

void UPauseWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!SettingsWidgetInstance && SettingsWidgetClass)
	{
		SettingsWidgetInstance = CreateWidget<USettingsWidget>(GetWorld(), SettingsWidgetClass);
		SettingsWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
		SettingsWidgetInstance->OnSettingsClosed.AddDynamic(this, &UPauseWidget::ShowMenu);
	}

	if (ResumeButton)
	{
		ResumeButton->OnClicked().AddDynamic(this, &UPauseWidget::OnResumeButtonClicked);
	}
	if (SettingsButton)
	{
		SettingsButton->OnClicked().AddDynamic(this, &UPauseWidget::OnSettingsButtonClicked);
	}
	if (HelpButton)
	{
		HelpButton->OnClicked().AddDynamic(this, &UPauseWidget::OnHelpButtonClicked);
	}
	if (ToMenuButton)
	{
		ToMenuButton->OnClicked().AddDynamic(this, &UPauseWidget::OnToMenuButtonClicked);
	}
}

void UPauseWidget::OnResumeButtonClicked()
{
	if (OnResumeRequested.IsBound())
	{
		OnResumeRequested.Execute();
	}
}

void UPauseWidget::OnSettingsButtonClicked()
{
	if (SettingsWidgetInstance)
	{
		if (!SettingsWidgetInstance->IsInViewport())
		{
			SettingsWidgetInstance->AddToViewport(13);
		}

		HideMenu();
		SettingsWidgetInstance->HandleOpenWindow();
	}
}

void UPauseWidget::OnHelpButtonClicked()
{
	if (!HelpWidgetInstance && HelpWidgetClass)
	{
		HelpWidgetInstance = CreateWidget<UHelpWidget>(GetWorld(), HelpWidgetClass);
		HelpWidgetInstance->AddToViewport(13);
		HelpWidgetInstance->OnHelpClosed.AddDynamic(this, &UPauseWidget::ShowMenu);
	}

	HideMenu();
}

void UPauseWidget::HideMenu()
{
	UE_LOG(LogTemp, Warning, TEXT("HideMenu() called"));

	//PlayAnimation(HideMenuAnim);
	PauseMenuOverlay->SetVisibility(ESlateVisibility::Hidden);
}

void UPauseWidget::ShowMenu()
{
	UE_LOG(LogTemp, Warning, TEXT("ShowMenu() called"));

	//PlayAnimation(ShowMenuAnim);
	PauseMenuOverlay->SetVisibility(ESlateVisibility::Visible);

	if (HelpWidgetInstance)
	{
		HelpWidgetInstance->OnHelpClosed.RemoveDynamic(this, &UPauseWidget::ShowMenu);
		HelpWidgetInstance = nullptr;
	}
}

void UPauseWidget::OnToMenuButtonClicked()
{
	AC4SurvivalGameMode* GM = Cast<AC4SurvivalGameMode>(UGameplayStatics::GetGameMode(this));
	if (GM)
	{
		GM->ReturnToMenu();
	}
}

