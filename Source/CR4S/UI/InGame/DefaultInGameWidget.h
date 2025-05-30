#pragma once

#include "Blueprint/UserWidget.h"
#include "Game/System/WorldTimeManager.h"
#include "DefaultInGameWidget.generated.h"

class UProgressBar;
class UCharacterStatusWidget;
class UBaseStatusComponent;
class UTimeDisplayWidget;

UCLASS()
class CR4S_API UDefaultInGameWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

#pragma region Initialize
public:
	void InitializeStatusWidget(UBaseStatusComponent* InComponent, bool bIsRobot);
#pragma endregion
	
#pragma region UpdateWidget
public:
	void UpdateHPWidget(const float InPercentage);
	void UpdateResourceWidget(const float InPercentage);
	void UpdateEnergyWidget(const float InPercentage);
	void UpdateStunWidget(const float InPercentage);
	void UpdateHungerWidget(const float InPercentage);
	void UpdateTimeWidget(FWorldTimeData CurrentTimeData);
#pragma endregion

#pragma region Widgets
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<UCharacterStatusWidget> StatusWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<UProgressBar> HungerWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTimeDisplayWidget> TimeDisplayWidget;
#pragma endregion
	
};
