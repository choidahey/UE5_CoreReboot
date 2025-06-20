#pragma once

#include "Blueprint/UserWidget.h"
#include "CharacterEnvironmentStatusWidget.generated.h"

class UEnvironmentalStatusComponent;
class UProgressBar;
class UTextBlock;
class AEnvironmentManager;

UCLASS()
class CR4S_API UCharacterEnvironmentStatusWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void OnTemperatureChanged(float NewTemperature);
	UFUNCTION()
	void OnHumidityChanged(float NewHumidity);
	UFUNCTION()
	void UpdateColdThreshold(float NewThreshold);
	UFUNCTION()
	void UpdateHeatThreshold(float NewThreshold);
	UFUNCTION()
	void UpdateHumidityThreshold(float NewThreshold);
	UFUNCTION()
	void UpdateTemperatureProgressBar();
	UFUNCTION()
	void UpdateHumidityProgressBar();
	bool InitializeWidget(ACharacter* NewPawn);
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;


	UPROPERTY(meta = (BindWidget))
	UProgressBar* TemperatureProgressBar;
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HumidityProgressBar;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* MinTempText;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* MaxTempText;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* MinHuText;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* MaxHuText;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* CurrentTempText;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* CurrentHuText;
	TWeakObjectPtr<AEnvironmentManager> EnvironmentManager;

	bool bIsBoundToEnvironment = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<ACharacter> CurrentCharacter;

};
