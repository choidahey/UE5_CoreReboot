#include "UI/InGame/MarkerWidget.h"
#include "UI/InGame/PinSettingsWidget.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

void UMarkerWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

FReply UMarkerWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (OnPinSelected.IsBound())
	{
		OnPinSelected.Execute(this);
	}

	return FReply::Handled();
}

void UMarkerWidget::NativeOnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	Super::NativeOnMouseEnter(MyGeometry, MouseEvent);

	PlayAnimation(Hover);
	bIsHovered = true;
}

void UMarkerWidget::NativeOnMouseLeave(const FPointerEvent& MouseEvent)
{
	Super::NativeOnMouseLeave(MouseEvent);

	if (!bIsSelected)
	{
		PlayAnimation(UnHover);
		bIsHovered = false;
	}
}

void UMarkerWidget::SetPinImage(FSlateBrush& Brush)
{
	if (PinImage)
	{
		PinImage->SetBrush(Brush);
		PinImage->SetBrushSize(FVector2D(50.0f, 50.0f));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("IconImage is null in UMarkerWidget::SetIconImage"));
	}
}

void UMarkerWidget::SetSelected()
{
	if (!bIsHovered)
	{
		PlayAnimation(Hover);
		bIsHovered = true;
	}

	PinSettingsRef->SetWidgetData(PinData.IconIndex, PinData.PinName);
	PinSettingsRef->SetRemoveButtonVisibility(ESlateVisibility::Visible);

	if(ClickSound)
	{
		UGameplayStatics::PlaySound2D(this, ClickSound);
	}

	bIsSelected = true;
}

void UMarkerWidget::SetDeSelected()
{
	PlayAnimation(UnHover);
	bIsSelected = false;
}

void UMarkerWidget::RemovePin()
{
	OnPinSelected.Unbind();
	RemoveFromParent();
}

void UMarkerWidget::NativeDestruct()
{
	Super::NativeDestruct();
	OnPinSelected.Unbind();
}
