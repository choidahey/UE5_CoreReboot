#pragma once

#include "Blueprint/UserWidget.h"
#include "TimeDisplayWidget.generated.h"

class UTextBlock;
class USlider;

UCLASS()
class CR4S_API UTimeDisplayWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void UpdateTime(int32 Day, int32 Minute, int32 Second);

protected:

	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	USlider* SeasonSlider;
	UPROPERTY(meta = (BindWidget))
	USlider* TimeSlider;
private:
	int32 DayCycleLength = 0;
	int32 SeasonLength = 0;
};
