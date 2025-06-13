#include "UI/InGame/PinSettingsWidget.h"
#include "Components/Button.h"
#include "Components/EditableText.h"

void UPinSettingsWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	UserIconCount = IconTextures.Num();
	TArray<UButton*> Buttons = { Icon0, Icon1, Icon2, Icon3, Icon4 };

	int32 ApplyCount = FMath::Min(UserIconCount, Buttons.Num());

	for (int32 i = 0; i < ApplyCount; ++i)
	{
		if (IconTextures[i] && Buttons[i])
		{
			FSlateBrush NormalBrush;
			NormalBrush.SetResourceObject(IconTextures[i]);
			NormalBrush.ImageSize = FVector2D(70.f, 70.f);

			FSlateBrush HoveredBrush = NormalBrush;
			HoveredBrush.TintColor = FLinearColor(1.2f, 1.2f, 1.2f, 1.0f);

			FSlateBrush PressedBrush = NormalBrush;
			PressedBrush.TintColor = FLinearColor(0.7f, 0.7f, 0.7f, 1.0f);

			FButtonStyle NewStyle = Buttons[i]->WidgetStyle;
			NewStyle.Normal = NormalBrush;
			NewStyle.Hovered = HoveredBrush;
			NewStyle.Pressed = PressedBrush;

			Buttons[i]->SetStyle(NewStyle);
		}
	}
}



void UPinSettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (AddPinButton)
	{
		AddPinButton->OnClicked.AddDynamic(this, &UPinSettingsWidget::HandleAddPinClicked);
	}
	if (RemovePinButton)
	{
		RemovePinButton->OnClicked.AddDynamic(this, &UPinSettingsWidget::HandleRemovePinClicked);
	}

	if (Icon0) Icon0->OnClicked.AddDynamic(this, &UPinSettingsWidget::HandleIcon0Clicked);
	if (Icon1) Icon1->OnClicked.AddDynamic(this, &UPinSettingsWidget::HandleIcon1Clicked);
	if (Icon2) Icon2->OnClicked.AddDynamic(this, &UPinSettingsWidget::HandleIcon2Clicked);
	if (Icon3) Icon3->OnClicked.AddDynamic(this, &UPinSettingsWidget::HandleIcon3Clicked);
	if (Icon4) Icon4->OnClicked.AddDynamic(this, &UPinSettingsWidget::HandleIcon4Clicked);

}

FReply UPinSettingsWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return FReply::Handled();
}

void UPinSettingsWidget::HandleAddPinClicked()
{
	OnAddPinClicked.Broadcast();
}

void UPinSettingsWidget::HandleRemovePinClicked()
{
	OnRemovePinClicked.Broadcast();
}

void UPinSettingsWidget::HandleIcon0Clicked() { HandleIconClicked(0); }
void UPinSettingsWidget::HandleIcon1Clicked() { HandleIconClicked(1); }
void UPinSettingsWidget::HandleIcon2Clicked() { HandleIconClicked(2); }
void UPinSettingsWidget::HandleIcon3Clicked() { HandleIconClicked(3); }
void UPinSettingsWidget::HandleIcon4Clicked() { HandleIconClicked(4); }

void UPinSettingsWidget::HandleIconClicked(int32 IconIndex)
{
	SelectedIconIndex = IconIndex;
	OnIconClicked.Broadcast(IconIndex);
}

FSlateBrush UPinSettingsWidget::GetIconBrush(int32 Index) const
{
	if (IconTextures.IsValidIndex(Index) && IconTextures[Index])
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(IconTextures[Index]);
		Brush.ImageSize = FVector2D(70.f, 70.f);
		return Brush;
	}

	return FSlateBrush();
}

FString UPinSettingsWidget::GetPinName() const
{
	if (PinNameText)
	{
		return PinNameText->GetText().ToString();
	}
	return FString();
}

void UPinSettingsWidget::SetWidgetData(int32 Index, FString Name)
{
	if (PinNameText)
	{
		PinNameText->SetText(FText::FromString(Name));
	}

	// Update the icon based on the selected index
}

void UPinSettingsWidget::SetRemoveButtonVisibility(ESlateVisibility InVisibility)
{
	if (RemovePinButton)
	{
		RemovePinButton->SetVisibility(InVisibility);
	}
}

void UPinSettingsWidget::ResetWidgetData()
{
	if (PinNameText)
	{
		PinNameText->SetText(FText::GetEmpty());
	}

	SelectedIconIndex = 0; 
}

void UPinSettingsWidget::NativeDestruct()
{
		Super::NativeDestruct();
	if (AddPinButton)
	{
		AddPinButton->OnClicked.RemoveDynamic(this, &UPinSettingsWidget::HandleAddPinClicked);
	}
	if (RemovePinButton)
	{
		RemovePinButton->OnClicked.RemoveDynamic(this, &UPinSettingsWidget::HandleRemovePinClicked);
	}
	if (Icon0) Icon0->OnClicked.RemoveDynamic(this, &UPinSettingsWidget::HandleIcon0Clicked);
	if (Icon1) Icon1->OnClicked.RemoveDynamic(this, &UPinSettingsWidget::HandleIcon1Clicked);
	if (Icon2) Icon2->OnClicked.RemoveDynamic(this, &UPinSettingsWidget::HandleIcon2Clicked);
	if (Icon3) Icon3->OnClicked.RemoveDynamic(this, &UPinSettingsWidget::HandleIcon3Clicked);
	if (Icon4) Icon4->OnClicked.RemoveDynamic(this, &UPinSettingsWidget::HandleIcon4Clicked);
}