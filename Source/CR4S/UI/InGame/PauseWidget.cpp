#include "UI/InGame/PauseWidget.h"
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
			SettingsWidgetInstance->AddToViewport(11);
		}

		HideMenu();
		SettingsWidgetInstance->HandleOpenWindow();
	}
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
}

void UPauseWidget::OnToMenuButtonClicked()
{
	AC4SurvivalGameMode* GM = Cast<AC4SurvivalGameMode>(UGameplayStatics::GetGameMode(this));
	if (GM)
	{
		GM->ReturnToMenu();
	}
}

