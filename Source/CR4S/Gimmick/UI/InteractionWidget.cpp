#include "InteractionWidget.h"

#include "Components/TextBlock.h"

void UInteractionWidget::SetInteractionText(const FText& InInteractionText) const
{
	InteractionTextBlock->SetText(InInteractionText);
}
