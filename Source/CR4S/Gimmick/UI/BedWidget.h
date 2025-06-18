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
	void InitWidget(APlayerController* PlayerController, const bool bCanSleep);

private:
	UPROPERTY()
	TObjectPtr<ASurvivalHUD> SurvivalHUD;
	
#pragma endregion

#pragma region Sleep

public:
	UFUNCTION()
	void HandleSleep();
	
#pragma endregion

#pragma region ToggleWidget

private:
	UFUNCTION()
	void RemoveWidget();
	
#pragma endregion 
	
#pragma region BindWidget

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SaveButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SleepButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CancelButton;
	
#pragma endregion
};
