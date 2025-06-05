#include "UI/InGame/TimeDisplayWidget.h"
#include "Game/System/WorldTimeManager.h"
#include "Game/System/SeasonManager.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"

void UTimeDisplayWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UWorldTimeManager* TimeManager = GetWorld()->GetSubsystem<UWorldTimeManager>();
	if (TimeManager)
	{
		DayCycleLength = TimeManager->GetDayCycleLength();
	}

	USeasonManager* SeasonManager = GetWorld()->GetSubsystem<USeasonManager>();
	if (SeasonManager)
	{
		SeasonLength = SeasonManager->GetSeasonLength();
	}
}

void UTimeDisplayWidget::UpdateTime(int32 Day, int32 Minute, int32 Second)
{
	FString DayString = FString::FromInt(Day);
	FString MinuteString = FString::FromInt(Minute);
	FString SecondString = FString::FromInt(Second);

	DayNumber->SetText(FText::FromString(DayString));
	MinuteNumber->SetText(FText::FromString(MinuteString));
	SecondNumber->SetText(FText::FromString(SecondString));

	float TimeValue = ((Minute * 60.0f) + Second) / (DayCycleLength*60.0f);
	TimeValue = FMath::Clamp(TimeValue, 0.0f, 1.0f);
	TimeSlider->SetValue(TimeValue);

	float YearLength = 4.0f * SeasonLength;
	float DayInYear = FMath::Fmod(Day, YearLength); 

	float DayValue = DayInYear / YearLength;
	SeasonSlider->SetValue(DayValue);
}