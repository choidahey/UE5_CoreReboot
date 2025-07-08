#include "ItemNotificationContainerWidget.h"

#include "ItemNotificationWidget.h"
#include "Animation/WidgetAnimation.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

void UItemNotificationContainerWidget::AddItemNotification(const FAddItemData& AddItemData)
{
	if (ActiveNotifications.Num() >= MaxNotifications)
	{
		RemoveOldestNotification();
	}
	
	if (NotificationWidgetClass)
	{
		UItemNotificationWidget* NewNotification = CreateWidget<UItemNotificationWidget>(GetWorld(), NotificationWidgetClass);
        
		if (IsValid(NewNotification))
		{
			NewNotification->SetAddItemData(AddItemData);
			
			if (IsValid(NotificationContainer))
			{
				UVerticalBoxSlot* VerticalBoxSlot = NotificationContainer->AddChildToVerticalBox(NewNotification);
				if (IsValid(VerticalBoxSlot))
				{
					VerticalBoxSlot->SetHorizontalAlignment(HAlign_Right);
					VerticalBoxSlot->SetVerticalAlignment(VAlign_Bottom);
				}
			}
			
			ActiveNotifications.Add(NewNotification);

			NewNotification->OnNotificationRemoved.AddDynamic(this,&ThisClass::RemoveNotification);
			
			NewNotification->PlaySlideInAnim();
		}
	}
}

void UItemNotificationContainerWidget::RemoveOldestNotification()
{
	if (ActiveNotifications.Num() > 0)
	{
		UItemNotificationWidget* OldestNotification = ActiveNotifications[0];
		if (IsValid(OldestNotification))
		{
			OldestNotification->StartFadeOut();
			ActiveNotifications.Remove(OldestNotification);	
		}
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UItemNotificationContainerWidget::RemoveNotification(UItemNotificationWidget* NotificationWidget)
{
	if (NotificationWidget && ActiveNotifications.Contains(NotificationWidget))
	{
		if (IsValid(NotificationContainer))
		{
			NotificationContainer->RemoveChild(NotificationWidget);
		}
		
		NotificationWidget->RemoveFromParent();
	}
}
