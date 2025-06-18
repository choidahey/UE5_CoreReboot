#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BedWidget.generated.h"

class ASurvivalHUD;
class UButton;

UCLASS()
class CR4S_API UBedWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region Initialize

public:
	UFUNCTION(BlueprintCallable, Category = "Initialize")
	void InitWidget();

private:
	bool CanSleep() const;
	
	UPROPERTY()
	TObjectPtr<ASurvivalHUD> SurvivalHUD;
	
#pragma endregion

#pragma region Sleep

private:
	UFUNCTION()
	void PlaySleepingAnimation();
	UFUNCTION()
	void PlayAwakingAnimation();

	UFUNCTION()
	void HandleAnimationFinished();
	
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* SleepingAnim;

	bool bIsSleeping;
	
#pragma endregion

#pragma region ModifyStat

private:
	void ModifyStat(float SleepingTime) const;
	float CalculateHealthRecovery(const float SleepingTime) const;
	float CalculateHungerReduction(const float SleepingTime) const;

	UPROPERTY(EditDefaultsOnly, Category = "ModifyStat")
	float MaxSleepTime = 1000;
	
#pragma endregion 

#pragma region ToggleWidget

private:
	UFUNCTION()
	void RemoveWidget();

	void CloseButtonWidgets() const;
	
#pragma endregion 
	
#pragma region BindWidget

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> ButtonContainer;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SaveButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SleepButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CancelButton;


	UPROPERTY(EditDefaultsOnly, Category = "Time")
	float AddTime;
	
#pragma endregion

#pragma region Delegate

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWidgetClose);
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnWidgetClose OnWidgetClose;
	
#pragma endregion 
};
