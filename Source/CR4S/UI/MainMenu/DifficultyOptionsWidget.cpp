#include "UI/MainMenu/DifficultyOptionsWidget.h"
#include "UI/Common/LoadingWidget.h"
#include "UI/MainMenu/MainMenuWidget.h"
#include "UI/Common/BaseWindowWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Game/System/AudioManager.h"

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

void UDifficultyOptionsWidget::HandleOpenWindow()
{
	SetVisibility(ESlateVisibility::Visible);
	PlayAnimation(FadeIn);
}


void UDifficultyOptionsWidget::HandleCloseWindow()
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

void UDifficultyOptionsWidget::HandleOpenLevel()
{
	if (!LoadingWidgetInstance && LoadingWidgetClass)
	{
		LoadingWidgetInstance = CreateWidget<ULoadingWidget>(GetWorld(), LoadingWidgetClass);
	}	

	if (UAudioManager* AudioManager = GetGameInstance()->GetSubsystem<UAudioManager>())
	{
<<<<<<< HEAD
		MainMenuWidgetRef->FadeOutBGM(1.0f); // 1�� ���̵� �ƿ�
=======
		AudioManager->StopBGM();
>>>>>>> 1f091cf5a109ae66a25a37b9117eb0ad23417219
	}


	LoadingWidgetInstance->AddToViewport(10);
	
	// OpenLevel after 0.5 sec Delay, for Prototype
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		this,
		&UDifficultyOptionsWidget::OpenSurvivalLevel,
		1.0f,
		false
	);
}


void UDifficultyOptionsWidget::OpenSurvivalLevel()
{
	UGameplayStatics::OpenLevel(this, FName("MainMap"));
}