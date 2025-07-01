#include "HelperBotManagementWidget.h"

#include "HelperBotRepairWidget.h"
#include "Components/Button.h"

void UHelperBotManagementWidget::InitWidget(UHelperBotRepairWidget* NewHelperBotRepairWidget)
{
	HelperBotRepairWidget = NewHelperBotRepairWidget;

	if (IsValid(BotRepairButton))
	{
		BotRepairButton->OnClicked.AddUniqueDynamic(this, &ThisClass::OpenHelperBotRepairWidget);
	}
}

void UHelperBotManagementWidget::OpenWidget()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UHelperBotManagementWidget::CloseWidget()
{
	SetVisibility(ESlateVisibility::Hidden);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UHelperBotManagementWidget::OpenHelperBotRepairWidget()
{
	if (IsValid(HelperBotRepairWidget))
	{
		HelperBotRepairWidget->OpenWidget();
	}
}
