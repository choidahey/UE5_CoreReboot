#pragma once

#include "Blueprint/UserWidget.h"
#include "DifficultyOptionsWidget.generated.h"

class UButton;
class ULoadingWidget;

UCLASS()
class CR4S_API UDifficultyOptionsWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnBackButtonClicked();
	UFUNCTION()
	void OnApplyStartButtonClicked();

	UPROPERTY(meta = (BindWidget))
	UButton* BackButton;
	UPROPERTY(meta = (BindWidget))
	UButton* ApplyStartButton;
	
	//Just for Prototype
	UPROPERTY()
	ULoadingWidget* LoadingWidgetInstance;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<ULoadingWidget> LoadingWidgetClass;
	void OpenSurvivalLevel();
};
