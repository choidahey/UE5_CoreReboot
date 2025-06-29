#include "UI/InGame/PauseWidget.h"
#include "UI/Common/ButtonWidget.h"
#include "Game/GameMode/C4SurvivalGameMode.h"
#include "Kismet/GameplayStatics.h"

void UPauseWidget::NativeConstruct()
{
	Super::NativeConstruct();

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

}

void UPauseWidget::OnToMenuButtonClicked()
{
	AC4SurvivalGameMode* GM = Cast<AC4SurvivalGameMode>(UGameplayStatics::GetGameMode(this));
	if (GM)
	{
		GM->ReturnToMenu();
	}
}

