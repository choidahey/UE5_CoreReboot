#include "UI/InGame/SurvivalHUD.h"
#include "Blueprint/UserWidget.h"
#include "UI/InGame/TimeDisplayWidget.h"

void ASurvivalHUD::BeginPlay()
{
    Super::BeginPlay();

    InGameWidget=CreateAndAddWidget<UDefaultInGameWidget>(InGameWidgetClass,0,ESlateVisibility::Visible);
    TimeDisplayWidget=CreateAndAddWidget<UTimeDisplayWidget>(TimeDisplayWidgetClass,0,ESlateVisibility::Visible);
    // if (TimeDisplayWidgetClass)
    // {
    //     TimeDisplayWidget = CreateWidget<UTimeDisplayWidget>(GetWorld(), TimeDisplayWidgetClass);
    //     if (TimeDisplayWidget)
    //     {
    //         TimeDisplayWidget->AddToViewport();
    //     }
    // }
}

void ASurvivalHUD::SetTime(FWorldTimeData CurrentTimeData)
{
    TimeDisplayWidget->UpdateTime(CurrentTimeData.Day, CurrentTimeData.Minute, CurrentTimeData.Second);
}