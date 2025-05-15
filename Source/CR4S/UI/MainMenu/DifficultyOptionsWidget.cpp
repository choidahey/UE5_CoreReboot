#include "UI/MainMenu/DifficultyOptionsWidget.h"
#include "UI/Common/LoadingWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"

void UDifficultyOptionsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BackButton)
	{
		BackButton->OnClicked.AddUniqueDynamic(this, &UDifficultyOptionsWidget::OnBackButtonClicked);
	}
	if (ApplyStartButton)
	{
		ApplyStartButton->OnClicked.AddUniqueDynamic(this, &UDifficultyOptionsWidget::OnApplyStartButtonClicked);
	}

	if (!LoadingWidgetInstance && LoadingWidgetClass)
	{
		LoadingWidgetInstance = CreateWidget<ULoadingWidget>(GetWorld(), LoadingWidgetClass);
	}
}

void UDifficultyOptionsWidget::OnBackButtonClicked()
{
	RemoveFromParent();
}

void UDifficultyOptionsWidget::OnApplyStartButtonClicked()
{
	LoadingWidgetInstance->AddToViewport(10);

	// OpenLevel after 0.5 sec Delay, for Prototype
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		this,
		&UDifficultyOptionsWidget::OpenSurvivalLevel,
		0.5f,
		false
	);
}

void UDifficultyOptionsWidget::OpenSurvivalLevel()
{
	UGameplayStatics::OpenLevel(this, FName("SurvivalLevel"));
}