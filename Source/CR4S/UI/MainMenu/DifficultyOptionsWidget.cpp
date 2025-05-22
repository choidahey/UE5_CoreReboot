#include "UI/MainMenu/DifficultyOptionsWidget.h"
#include "UI/Common/LoadingWidget.h"
#include "UI/Common/BaseWindowWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"

void UDifficultyOptionsWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (!WindowWidget->OnApplyClicked.IsAlreadyBound(this, &UDifficultyOptionsWidget::HandleOpenLevel))
	{
		WindowWidget->OnApplyClicked.AddDynamic(this, &UDifficultyOptionsWidget::HandleOpenLevel);
	}

	if (!WindowWidget->OnBackClicked.IsAlreadyBound(this, &UDifficultyOptionsWidget::HandleCloseWindow))
	{
		WindowWidget->OnBackClicked.AddDynamic(this, &UDifficultyOptionsWidget::HandleCloseWindow);
	}
}

void UDifficultyOptionsWidget::HandleCloseWindow()
{
	RemoveFromParent();
}

void UDifficultyOptionsWidget::HandleOpenLevel()
{
	if (!LoadingWidgetInstance && LoadingWidgetClass)
	{
		LoadingWidgetInstance = CreateWidget<ULoadingWidget>(GetWorld(), LoadingWidgetClass);
	}	

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