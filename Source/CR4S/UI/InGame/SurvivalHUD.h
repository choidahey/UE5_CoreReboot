#pragma once

#include "GameFramework/HUD.h"
#include "Game/System/WorldTimeManager.h"
#include "SurvivalHUD.generated.h"

class UTimeDisplayWidget;

UCLASS()
class CR4S_API ASurvivalHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	void SetTime(FWorldTimeData CurrentTimeData);

protected:
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UTimeDisplayWidget> TimeDisplayWidgetClass;

private:
	virtual void BeginPlay() override;
	UTimeDisplayWidget* TimeDisplayWidget;
};
