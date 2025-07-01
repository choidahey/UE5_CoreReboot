#include "UI/Common/NotificationWidget.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"

void UNotificationWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetVisibility(ESlateVisibility::Collapsed);
}

void UNotificationWidget::ShowNotification(const FText& Text, float Duration)
{
	SetNotificationText(Text);
	PlayAnimation(FadeIn);
	SetVisibility(ESlateVisibility::Visible);
	if (Duration > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(AutoDismissHandle, this, &UNotificationWidget::OnAutoDismiss, Duration, false);
	}
}

void UNotificationWidget::SetNotificationText(const FText& Text)
{
	if (NotificationText)
	{
		NotificationText->SetText(Text);
	}
}


void UNotificationWidget::OnAutoDismiss()
{
	PlayAnimation(FadeOut);

	GetWorld()->GetTimerManager().ClearTimer(AutoDismissHandle);

	if (FadeOut)
	{
		float Duration = FadeOut->GetEndTime();
		GetWorld()->GetTimerManager().SetTimer(
			RemoveHandle,
			this,
			&UNotificationWidget::RemoveFromParent,
			Duration,
			false
		);
	}
	else
	{
		RemoveFromParent();
	}
}
