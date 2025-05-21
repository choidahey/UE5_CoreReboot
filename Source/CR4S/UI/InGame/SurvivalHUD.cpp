#include "UI/InGame/SurvivalHUD.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
void ASurvivalHUD::BeginPlay()
{
    Super::BeginPlay();

    InGameWidget=CreateAndAddWidget<UDefaultInGameWidget>(InGameWidgetClass,0,ESlateVisibility::Visible);
    PauseWidget = CreateAndAddWidget<UPauseWidget>(PauseWidgetClass, 10, ESlateVisibility::Collapsed);
    PauseWidget->OnResumeRequested.BindUObject(this, &ASurvivalHUD::HandlePauseToggle);

    // Add to InGameWidget later
    TimeDisplayWidget=CreateAndAddWidget<UTimeDisplayWidget>(TimeDisplayWidgetClass,0,ESlateVisibility::Visible);
}

// Add to InGameWidget later
void ASurvivalHUD::SetTime(FWorldTimeData CurrentTimeData)
{
    TimeDisplayWidget->UpdateTime(CurrentTimeData.Day, CurrentTimeData.Minute, CurrentTimeData.Second);
}

void ASurvivalHUD::HandlePauseToggle()
{
    const bool bIsVisible = PauseWidget->GetVisibility() == ESlateVisibility::Visible;

    APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
    if (PC)
    {
        if (bIsVisible)
        {
            UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
            PC->SetInputMode(FInputModeGameOnly());
            PC->bShowMouseCursor = false;
        }
        else
        {
            FInputModeGameAndUI InputMode;

            //*Include If Buttons can be Selected by Keyboard*//
            //PauseWidget->SetIsFocusable(true);
            //InputMode.SetWidgetToFocus(PauseWidget-());>TakeWidget

            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.0f);
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = true;
        }
    }

    ToggleWidget(PauseWidget);
}

void ASurvivalHUD::ToggleWidget(UUserWidget* Widget)
{
    if (!Widget)
        return;

    const bool bIsVisible = Widget->GetVisibility() == ESlateVisibility::Visible;

    if (bIsVisible)
    {
        HideWidget(Widget);
    }
    else
    {
        Widget->SetVisibility(ESlateVisibility::Visible);
        ActiveWidgets.Add(Widget);
    }
}

void ASurvivalHUD::HideWidget(UUserWidget* TargetWidget)
{
    if (TargetWidget)
    {
        if (ActiveWidgets.Contains(TargetWidget) && TargetWidget->GetVisibility() == ESlateVisibility::Visible)
        {
            TargetWidget->SetVisibility(ESlateVisibility::Collapsed);
            ActiveWidgets.Remove(TargetWidget);
        }
    }
    else
    {
        if (ActiveWidgets.Num() > 0)
        {
            UUserWidget* LastWindow = ActiveWidgets.Last();
            if (LastWindow && LastWindow->GetVisibility() == ESlateVisibility::Visible)
            {
                LastWindow->SetVisibility(ESlateVisibility::Collapsed);
            }

            ActiveWidgets.Pop();
        }
    }
}

