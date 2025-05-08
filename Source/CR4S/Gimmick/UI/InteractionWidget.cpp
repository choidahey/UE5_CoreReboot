#include "InteractionWidget.h"

#include "Components/TextBlock.h"

void UInteractionWidget::InitWidget(const FString& InInteractionText) const
{
	InteractionTextBlock->SetText(FText::FromString(InInteractionText));
}
