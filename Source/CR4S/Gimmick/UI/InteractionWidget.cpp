#include "InteractionWidget.h"

#include "Components/TextBlock.h"

void UInteractionWidget::InitWidget(const FText& InInteractionText) const
{
	InteractionTextBlock->SetText(InInteractionText);
}
