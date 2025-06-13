#include "UI/InGame/SurvivalHUD.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void ASurvivalHUD::BeginPlay()
{
	Super::BeginPlay();

	InGameWidget = CreateAndAddWidget<UDefaultInGameWidget>(InGameWidgetClass, 0, ESlateVisibility::Visible);

	PauseWidget = CreateAndAddWidget<UPauseWidget>(PauseWidgetClass, 11, ESlateVisibility::Collapsed);
	PauseWidget->OnResumeRequested.BindUObject(this, &ASurvivalHUD::HandlePauseToggle);

	WorldMapWidget = CreateAndAddWidget<UWorldMapWidget>(WorldMapWidgetClass, 11, ESlateVisibility::Collapsed);
}

void ASurvivalHUD::PlayEndingSequence()
{
	EndingWidget = CreateAndAddWidget<UEndingSummaryWidget>(EndingSummaryWidgetClass, 11, ESlateVisibility::Visible);
	SetInputMode(ESurvivalInputMode::UIOnly, EndingWidget, true);
}

void ASurvivalHUD::HandlePauseToggle()
{
	const bool bIsVisible = PauseWidget->GetVisibility() == ESlateVisibility::Visible;

	if (bIsVisible)
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
		SetInputMode(ESurvivalInputMode::GameOnly, nullptr, false);
	}
	else
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.0001f);
		SetInputMode(ESurvivalInputMode::GameAndUI, PauseWidget, true);
	}

	ToggleWidget(PauseWidget);
}

void ASurvivalHUD::HandleMapToggle()
{
	const bool bIsVisible = WorldMapWidget->GetVisibility() == ESlateVisibility::Visible;

	if (bIsVisible)
	{
		SetInputMode(ESurvivalInputMode::GameOnly, nullptr, false);
		WorldMapWidget->HideMarker();
		WorldMapWidget->PlayCloseMapSound();
	}
	else
	{
		SetInputMode(ESurvivalInputMode::GameAndUI, WorldMapWidget, true);
		WorldMapWidget->UpdateWorldMap();
		WorldMapWidget->PlayOpenMapSound();
	}

	ToggleWidget(WorldMapWidget);
}

void ASurvivalHUD::ToggleWidget(UUserWidget* Widget)
{
	if (!Widget)
		return;

	const bool bIsVisible = Widget->GetVisibility() == ESlateVisibility::Visible;

	if (bIsVisible)
	{
		Widget->SetVisibility(ESlateVisibility::Collapsed);
		ChangeWidgetZOrder(Widget, 0);
	}
	else
	{
		Widget->SetVisibility(ESlateVisibility::Visible);
		ChangeWidgetZOrder(Widget, 11);
	}
}

void ASurvivalHUD::SetInputMode(ESurvivalInputMode Mode, UUserWidget* FocusWidget, bool bShowCursor, bool bLockMouse)
{
	APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
	if (!PC) return;

	switch (Mode)
	{
	case ESurvivalInputMode::GameOnly:
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		break;
	}

	case ESurvivalInputMode::GameAndUI:
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(
			bLockMouse ? EMouseLockMode::LockAlways : EMouseLockMode::DoNotLock
		);

		if (FocusWidget)
		{
			if (!FocusWidget->IsFocusable())
			{
				FocusWidget->SetIsFocusable(true);
			}

			InputMode.SetWidgetToFocus(FocusWidget->TakeWidget());
		}

		PC->SetInputMode(InputMode);
		break;
	}

	case ESurvivalInputMode::UIOnly:
	{
		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(
			bLockMouse ? EMouseLockMode::LockAlways : EMouseLockMode::DoNotLock
		);

		if (FocusWidget)
		{
			if (!FocusWidget->IsFocusable())
			{
				FocusWidget->SetIsFocusable(true);
			}

			InputMode.SetWidgetToFocus(FocusWidget->TakeWidget());
		}

		PC->SetInputMode(InputMode);
		break;
	}
	}

	PC->bShowMouseCursor = bShowCursor;
}

void ASurvivalHUD::ChangeWidgetZOrder(UUserWidget* Widget, int32 NewZOrder)
{
	if (!Widget || !Widget->IsInViewport())
		return;

	Widget->AddToViewport(NewZOrder); 
}
