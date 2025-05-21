#pragma once

#include "Blueprint/UserWidget.h"
#include "DifficultyOptionsWidget.generated.h"

class UButton;
class ULoadingWidget;
class UBaseWindowWidget;

UCLASS()
class CR4S_API UDifficultyOptionsWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UFUNCTION()
	void HandleOpenLevel();
	UFUNCTION()
	void HandleCloseWindow();

	virtual void NativeConstruct() override;

	void OpenSurvivalLevel();

	UPROPERTY(meta = (BindWidget))
	UBaseWindowWidget* WindowWidget;

	//Just for Prototype
	UPROPERTY()
	ULoadingWidget* LoadingWidgetInstance;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<ULoadingWidget> LoadingWidgetClass;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeIn;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeOut;
};
