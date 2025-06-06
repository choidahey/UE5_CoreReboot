#include "CraftingWidget.h"

void UCraftingWidget::InitWidget()
{
}

void UCraftingWidget::OpenWidget()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UCraftingWidget::CloseWidget()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
