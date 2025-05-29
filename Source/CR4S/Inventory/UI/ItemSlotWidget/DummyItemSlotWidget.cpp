#include "DummyItemSlotWidget.h"

#include "CR4S.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UDummyItemSlotWidget::SetDummy(UTexture2D* ItemIcon, const int32 ItemCount) const
{
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(ItemIcon)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(CountTextBlock)))
	{
		return;
	}

	IconImage->SetBrushFromTexture(ItemIcon);
	CountTextBlock->SetText(FText::AsNumber(ItemCount));
}
