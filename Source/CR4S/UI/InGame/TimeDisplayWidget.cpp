#include "UI/InGame/TimeDisplayWidget.h"
#include "Game/System/WorldTimeManager.h"
#include "Components/TextBlock.h"

void UTimeDisplayWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UTimeDisplayWidget::UpdateTime(int32 Day, int32 Minute, int32 Second)
{
	FString DayString = FString::FromInt(Day);
	FString MinuteString = FString::FromInt(Minute);
	FString SecondString = FString::FromInt(Second);

	DayNumber->SetText(FText::FromString(DayString));
	MinuteNumber->SetText(FText::FromString(MinuteString));
	SecondNumber->SetText(FText::FromString(SecondString));
}