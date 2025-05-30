#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HelperBotInfoWidget.generated.h"

class UTextBlock;

UCLASS()
class CR4S_API UHelperBotInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetHealth(float InCurrent, float InMax);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentHealth;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MaxHealth;
};
