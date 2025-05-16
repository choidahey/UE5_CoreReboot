#pragma once

#include "Blueprint/UserWidget.h"
#include "TimeDisplayWidget.generated.h"

class UTextBlock;

UCLASS()
class CR4S_API UTimeDisplayWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void UpdateTime(int32 Day, int32 Minute, int32 Second);

protected:

	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DayNumber;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MinuteNumber;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SecondNumber;

private:

};
