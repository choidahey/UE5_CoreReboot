#include "DummyItemSlotWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UDummyItemSlotWidget::SetDummy(UTexture2D* ItemIcon, const int32 ItemCount) const
{
	IconImage->SetBrushFromTexture(ItemIcon);
	CountTextBlock->SetText(FText::AsNumber(ItemCount));
}
