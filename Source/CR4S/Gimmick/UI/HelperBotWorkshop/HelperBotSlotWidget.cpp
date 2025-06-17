#include "HelperBotSlotWidget.h"

#include "CR4S.h"
#include "HelperBotManagementWidget.h"
#include "Blueprint/DragDropOperation.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Inventory/UI/ItemSlotWidget/BaseItemSlotWidget.h"

void UHelperBotSlotWidget::InitWidget(UHelperBotManagementWidget* HelperBotManagementWidget)
{
	if (!CR4S_VALIDATE(LogGimmickUI, IsValid(HelperBotManagementButton)) ||
		!CR4S_VALIDATE(LogGimmickUI, IsValid(HelperBotManagementWidget)) ||
		!CR4S_VALIDATE(LogGimmickUI, IsValid(OutLineBorder)))
	{
		return;
	}

	HelperBotManagementButton->OnClicked.AddUniqueDynamic(HelperBotManagementWidget,
															  &UHelperBotManagementWidget::OpenWidget);

	DefaultOutLineBorderColor = OutLineBorder->GetBrushColor();
}

void UHelperBotSlotWidget::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                             UDragDropOperation* InOperation)
{
	UBaseItemSlotWidget* ItemSlotWidget = Cast<UBaseItemSlotWidget>(InOperation->Payload);
	if (IsValid(ItemSlotWidget))
	{
		CR4S_Log(LogGimmickUI, Warning, TEXT("Have Item"));

		if (IsValid(OutLineBorder))
		{
			DefaultOutLineBorderColor.A = 1.0f;
			OutLineBorder->SetBrushColor(DefaultOutLineBorderColor);
		}
	}
	else
	{
		CR4S_Log(LogGimmickUI, Warning, TEXT("Have Not Item"));
	}

	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
}
