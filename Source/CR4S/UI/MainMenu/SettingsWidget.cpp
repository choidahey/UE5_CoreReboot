#include "UI/MainMenu/SettingsWidget.h"
#include "UI/Common/BaseWindowWidget.h"
#include "UI/MainMenu/MainMenuWidget.h"
#include "Components/Button.h"

void USettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!WindowWidget->OnBackClicked.IsAlreadyBound(this, &USettingsWidget::HandleCloseWindow))
	{
		WindowWidget->OnBackClicked.AddDynamic(this, &USettingsWidget::HandleCloseWindow);
	}
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
