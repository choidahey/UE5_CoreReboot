#pragma once

#include "Blueprint/UserWidget.h"
#include "Game/System/WorldTimeManager.h"
#include "DefaultInGameWidget.generated.h"

class UPlayerCharacterStatusComponent;

class UCharacterStatusWidget;
class UCharacterEnvironmentStatusWidget;
class UTimeDisplayWidget;

UCLASS()
class CR4S_API UDefaultInGameWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

#pragma region Initialize
public:
	void InitializeWidget(UPlayerCharacterStatusComponent* InComponent);
#pragma endregion
	
#pragma region UpdateWidget
public:
	void UpdateHPWidget(const float InPercentage);
	void UpdateHungerWidget(const float InPercentage);
	void UpdateStaminaWidget(const float InPercentage);
	void UpdateTimeWidget(FWorldTimeData CurrentTimeData);
#pragma endregion

#pragma region Widgets
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<UCharacterStatusWidget> StatusWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTimeDisplayWidget> TimeDisplayWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UCharacterEnvironmentStatusWidget> EnvironmentStatusWidget;

#pragma endregion
	
};
