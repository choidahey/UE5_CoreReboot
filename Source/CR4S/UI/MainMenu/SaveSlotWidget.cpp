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
	FDateTime Time = SlotData.SaveTime;
	FString TimeString = Time.ToString(TEXT("%Y-%m-%d %H:%M"));
	FText SaveText = FText::FromString(TimeString);
	SaveTimeText->SetText(SaveText);
	SlotNameText->SetText(FText::FromString(SlotData.SlotName));

	//if (SlotButton)
	//{
	//	SlotButton->SetIsEnabled(SlotData.bIsEnabled);
	//}
}

void USaveSlotWidget::PlayCreateAnimation()
{
	if (OnCreateAnimation)
	{
		PlayAnimation(OnCreateAnimation);
	}
}

