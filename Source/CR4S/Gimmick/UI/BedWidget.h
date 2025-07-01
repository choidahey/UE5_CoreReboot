#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BedWidget.generated.h"

class USaveGameManager;
class ABaseBuildingGimmick;
class ASurvivalHUD;
class UButton;

UCLASS()
class CR4S_API UBedWidget : public UUserWidget
{
	GENERATED_BODY()
	
#pragma region Initialize

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Initialize")
	void InitWidget(ABaseBuildingGimmick* BedGimmick);

private:
	UPROPERTY()
	TObjectPtr<ASurvivalHUD> SurvivalHUD;
	
#pragma endregion

#pragma region SaveGame

public:
	UFUNCTION()
	void SaveGame();

private:
	UPROPERTY(EditDefaultsOnly, Category = "SaveGame")
	FText SaveSuccessText;
	UPROPERTY(EditDefaultsOnly, Category = "SaveGame")
	FText SaveFailedText;
	UPROPERTY(EditDefaultsOnly, Category = "SaveGame")
	FText DoNotSleepText;
	UPROPERTY(EditDefaultsOnly, Category = "SaveGame")
	float MessageDuration = 2.f;
	
#pragma endregion 

#pragma region Sleep

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Sleep")
	void PlaySleepingAnimation();

	UFUNCTION(BlueprintCallable)
	void DoNotSleepNotify();
	
private:
	UFUNCTION()
	void PlayAwakingAnimation();
	UFUNCTION()
	void HandleAnimationFinished();

	bool CanSleep() const;
	
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
	float AllowedMaxSleepTime = 1000;
	
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
