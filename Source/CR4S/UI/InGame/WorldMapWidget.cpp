#include "UI/InGame/WorldMapWidget.h"
#include "UI/InGame/PinSettingsWidget.h"
#include "UI/InGame/MarkerWidget.h"
#include "Components/OverlaySlot.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/Button.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

void UWorldMapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);
	MapImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	if (PinSettingsButton)
	{
		PinSettingsButton->OnClicked.AddDynamic(this, &UWorldMapWidget::OnPinSettingsButtonClicked);
	}

	if (PinSettingsWidget)
	{
		PinSettingsWidget->OnAddPinClicked.AddDynamic(this, &UWorldMapWidget::HandleAddPin);
		PinSettingsWidget->OnIconClicked.AddDynamic(this, &UWorldMapWidget::HandleIconSelection);
		PinSettingsWidget->OnRemovePinClicked.AddDynamic(this, &UWorldMapWidget::HandleRemovePin);
	}

	if (MarkerIcon)
	{
		DefaultMarkerBrush = MarkerIcon->Brush;
	}
}

FReply UWorldMapWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton || !MapImage || !MarkerIcon)
	{
		return FReply::Unhandled();
	}

	PinSettingsWidget->ResetWidgetData();
	PinSettingsWidget->SetRemoveButtonVisibility(ESlateVisibility::Hidden);

	if(CurrentPin)
	{
		CurrentPin->SetDeSelected();

		const FPinData& CurrentData = CurrentPin->GetPinData();
		FSlateBrush OriginalBrush = PinSettingsWidget->GetIconBrush(CurrentData.IconIndex);
		CurrentPin->SetPinImage(OriginalBrush);
		
		CurrentPin = nullptr;
	}

	if (!bIsPinSettingsOpen)
	{
		PlayAnimation(PullPinSettings);
		bIsPinSettingsOpen = true;
	}

	ResetMarkerIcon();
	MarkerIcon->SetVisibility(ESlateVisibility::Visible);

	FVector2D LocalMousePos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	FVector2D AdjustedPos = LocalMousePos - FVector2D(22.5f, 44.f); // 마커 위치 보정
	CurrentMarkerPos = AdjustedPos;

	if (UOverlaySlot* MarkerSlot = Cast<UOverlaySlot>(MarkerIcon->Slot))
	{
		MarkerSlot->SetPadding(FMargin(AdjustedPos.X, AdjustedPos.Y, 0.f, 0.f));
	}

	return FReply::Handled();
}

void UWorldMapWidget::OnPinSettingsButtonClicked()
{
	TogglePinSettingsPanel();
}

void UWorldMapWidget::UpdateWorldMap()
{
	FindPlayerLocation();
	// Add Additional Map Features
}

void UWorldMapWidget::HideMarker()
{
	if (MarkerIcon)
	{
		MarkerIcon->SetVisibility(ESlateVisibility::Hidden);
	}
}


UMarkerWidget* UWorldMapWidget::AddMarkerByWorldLocation(const FVector& WorldLocation, int32 IconIndex, FText PinName, bool bEditable)
{
	FVector2D MapPos = WorldToMapPosition(WorldLocation);
	return AddMarkerByMapPosition(MapPos, IconIndex, PinName, bEditable);
}

UMarkerWidget* UWorldMapWidget::AddMarkerByMapPosition(const FVector2D& MapPosition, int32 IconIndex, FText PinName, bool bEditable)
{
	if (TryRemoveEditableMarkerAtPosition(MapPosition))
	{
		FString PinNameStr = PinName.ToString();
		return CreateAndAddMarker(MapPosition, IconIndex, PinNameStr, bEditable);
	}
	else
	{
		return nullptr;
	}
}

void UWorldMapWidget::DeleteMarker(UMarkerWidget* Marker)
{
	if (Marker && MapOverlay)
	{
		Marker->RemovePin();
	}
}

void UWorldMapWidget::FindPlayerLocation()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		APawn* Player = PC->GetPawn();
		if (!Player || !PlayerIcon || !MapImage) return;

		FVector PlayerWorldPos = Player->GetActorLocation();
		FVector2D MapPos = WorldToMapPosition(PlayerWorldPos);

		if (UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(PlayerIcon->Slot))
		{
			OverlaySlot->SetPadding(FMargin(MapPos.X, MapPos.Y, 0.f, 0.f));
		}

		float PlayerYaw = Player->GetActorRotation().Yaw;
		float IconRotation = PlayerYaw + 90.0f;
		PlayerIcon->SetRenderTransformAngle(IconRotation);
	}
}

FVector2D UWorldMapWidget::WorldToMapPosition(const FVector& WorldLocation) const
{
	if (!MapImage) return FVector2D::ZeroVector;

	FVector2D MapSize = MapImage->Brush.ImageSize;

	float XPercent = (WorldLocation.X - WorldMin.X) / (WorldMax.X - WorldMin.X);
	float YPercent = (WorldLocation.Y - WorldMin.Y) / (WorldMax.Y - WorldMin.Y);

	return FVector2D(MapSize.X * XPercent, MapSize.Y * YPercent);
}

void UWorldMapWidget::HandleAddPin()
{
	if (!TryRemoveEditableMarkerAtPosition(CurrentMarkerPos)) return;

	HideMarker();
	UMarkerWidget* NewMarker = CreateAndAddMarker(CurrentMarkerPos, PinSettingsWidget->GetSelectedIconIndex(), PinSettingsWidget->GetPinName(), true);

	if (NewMarker)
	{
		if (CurrentPin)
		{
			CurrentPin->SetDeSelected();
		}
		CurrentPin = NewMarker;
		NewMarker->SetSelected();
		PinSettingsWidget->SetRemoveButtonVisibility(ESlateVisibility::Visible);
	}
}

void UWorldMapWidget::HandleRemovePin()
{
	if (!CurrentPin) return;

	if (TryRemoveEditableMarkerAtPosition(CurrentPin->GetPinData().Position))
	{
		CurrentPin = nullptr;

		if (bIsPinSettingsOpen)
		{
			TogglePinSettingsPanel();
		}
	}
}


void UWorldMapWidget::HandleIconSelection(int32 IconIndex)
{
	MarkerIcon->SetBrush(PinSettingsWidget->GetIconBrush(IconIndex));
	MarkerIcon->SetBrushSize(FVector2D(50.f, 50.f));

	for (int32 i = SavedPins.Num() - 1; i >= 0; --i)
	{
		if (IsSamePosition(SavedPins[i].Position, CurrentMarkerPos))
		{
			if (UMarkerWidget* SelectedPin = FindMarkerAtPosition(CurrentMarkerPos))
			{
				if (!SelectedPin->bAllowEdit) return;

				SelectedPin->SetPinImage(MarkerIcon->Brush);
				break;
			}

			SavedPins[i].IconIndex = IconIndex;
			break;
		}
	}
}



void UWorldMapWidget::HandlePinSelectionChanged(UMarkerWidget* SelectedPin)
{
	if (CurrentPin && CurrentPin != SelectedPin)
	{
		CurrentPin->SetDeSelected();
	}

	if (!bIsPinSettingsOpen)
	{
		PlayAnimation(PullPinSettings);
		bIsPinSettingsOpen = true;
	}

	HideMarker();

	CurrentPin = SelectedPin;
	CurrentMarkerPos = SelectedPin->GetPinData().Position;
	PinSettingsWidget->SetSelectedIconIndex(SelectedPin->GetPinData().IconIndex);
	SelectedPin->SetSelected();
}

void UWorldMapWidget::ResetMarkerIcon()
{
	if (MarkerIcon)
	{
		MarkerIcon->SetBrush(DefaultMarkerBrush);
	}
}

UMarkerWidget* UWorldMapWidget::FindMarkerAtPosition(const FVector2D& Position) const
{
	if (!MapOverlay) return nullptr;

	const TArray<UWidget*>& OverlayChildren = MapOverlay->GetAllChildren();

	for (UWidget* Child : OverlayChildren)
	{
		if (UMarkerWidget* Marker = Cast<UMarkerWidget>(Child))
		{
			if (IsSamePosition(Marker->GetPinData().Position, Position))
			{
				return Marker;
			}
		}
	}

	return nullptr;
}


bool UWorldMapWidget::IsSamePosition(const FVector2D& A, const FVector2D& B, float Tolerance) const
{
	return A.Equals(B, Tolerance);
}

UMarkerWidget* UWorldMapWidget::CreateAndAddMarker(const FVector2D& MapPosition, int32 IconIndex, const FString& PinName, bool bEditable)
{
	if (!PinMarkerWidgetClass || !MapOverlay) return nullptr;

	UMarkerWidget* NewMarker = CreateWidget<UMarkerWidget>(GetWorld(), PinMarkerWidgetClass);
	if (!NewMarker) return nullptr;

	if (UOverlaySlot* LocalSlot = Cast<UOverlaySlot>(MapOverlay->AddChild(NewMarker)))
	{
		LocalSlot->SetPadding(FMargin(MapPosition.X, MapPosition.Y, 0.f, 0.f));
		LocalSlot->SetHorizontalAlignment(HAlign_Left);
		LocalSlot->SetVerticalAlignment(VAlign_Top);

		FPinData NewPinData;
		NewPinData.IconIndex = IconIndex;
		NewPinData.PinName = PinName;
		NewPinData.Position = MapPosition;

		NewMarker->SetPinData(NewPinData);

		NewMarker->SetParentRef(PinSettingsWidget);
		NewMarker->bAllowEdit = bEditable;

		FSlateBrush IconBrush = PinSettingsWidget->GetIconBrush(NewPinData.IconIndex);
		NewMarker->SetPinImage(IconBrush);
		NewMarker->OnPinSelected.BindUObject(this, &UWorldMapWidget::HandlePinSelectionChanged);

		SavedPins.Add(NewPinData);
		return NewMarker;
	}

	return nullptr;
}

bool UWorldMapWidget::TryRemoveEditableMarkerAtPosition(const FVector2D& Position)
{
	for (int32 i = SavedPins.Num() - 1; i >= 0; --i)
	{
		if (IsSamePosition(SavedPins[i].Position, Position))
		{
			if (UMarkerWidget* Marker = FindMarkerAtPosition(Position))
			{
				if (Marker->bAllowEdit)
				{
					Marker->RemovePin();
					SavedPins.RemoveAt(i);
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	}

	return true;
}

void UWorldMapWidget::TogglePinSettingsPanel()
{
	PlayAnimation(bIsPinSettingsOpen ? PushPinSettings : PullPinSettings);
	bIsPinSettingsOpen = !bIsPinSettingsOpen;
}

void UWorldMapWidget::PlayOpenMapSound()
{
	if (OpenMapSound)
	{
		UGameplayStatics::PlaySound2D(this, OpenMapSound);
	}
}

void UWorldMapWidget::PlayCloseMapSound()
{
	if (CloseMapSound)
	{
		UGameplayStatics::PlaySound2D(this, CloseMapSound);
	}
}

void UWorldMapWidget::NativeDestruct()
{
	Super::NativeDestruct();
	if (PinSettingsButton)
	{
		PinSettingsButton->OnClicked.RemoveAll(this);
	}
	if (PinSettingsWidget)
	{
		PinSettingsWidget->OnAddPinClicked.RemoveAll(this);
		PinSettingsWidget->OnIconClicked.RemoveAll(this);
		PinSettingsWidget->OnRemovePinClicked.RemoveAll(this);
	}
}