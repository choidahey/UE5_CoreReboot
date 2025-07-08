#include "ItemNotificationWidget.h"

#include "ItemNotificationContainerWidget.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UItemNotificationWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GetWorld()->GetTimerManager().SetTimer(
		AutoRemoveTimer,
		this,
		&ThisClass::StartFadeOut,
		AutoRemoveTime,
		false
	);

	if (IsValid(FadeOutAnim))
	{
		FWidgetAnimationDynamicEvent Delegate;
		Delegate.BindDynamic(this, &ThisClass::OnFadeOutComplete);
		BindToAnimationFinished(FadeOutAnim, Delegate);
	}
}

void UItemNotificationWidget::SetAddItemData(const FAddItemData& AddItemData)
{
	if (IsValid(ItemIcon) && IsValid(AddItemData.ItemIcon))
	{
		ItemIcon->SetBrushFromTexture(AddItemData.ItemIcon);
	}

	if (IsValid(ItemName))
	{
		ItemName->SetText(AddItemData.ItemName);
	}

	if (ItemCount)
	{
		ItemCount->SetText(FText::FromString(FString::Printf(TEXT("%d"), AddItemData.ItemCount)));
	}
}

void UItemNotificationWidget::PlaySlideInAnim()
{
	if (IsValid(SlideInAnim))
	{
		PlayAnimation(SlideInAnim);
	}
}

void UItemNotificationWidget::StartFadeOut()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(AutoRemoveTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(AutoRemoveTimer);
	}
	
	if (IsValid(FadeOutAnim))
	{
		PlayAnimation(FadeOutAnim);
	}
	else
	{
		OnFadeOutComplete();
	}
}

void UItemNotificationWidget::OnFadeOutComplete()
{
	OnNotificationRemoved.Broadcast(this);
}
