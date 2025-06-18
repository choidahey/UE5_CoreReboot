#include "UI/MainMenu/SaveSlotWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void USaveSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (SlotButton)
	{
		SlotButton->OnClicked.AddDynamic(this, &USaveSlotWidget::OnSlotButtonClicked);
	}
}

void USaveSlotWidget::OnSlotButtonClicked()
{

}

void USaveSlotWidget::UpdateSlotInfo(FSaveSlotMetaData SlotData)
{
	SlotNameText->SetText(FText::FromString(SlotData.SlotName));
	SaveTimeText->SetText(FText::AsDateTime(SlotData.SaveTime));


	//if (SlotButton)
	//{
	//	SlotButton->SetIsEnabled(SlotData.bIsEnabled);
	//}
}

