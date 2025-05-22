#pragma once

#include "Blueprint/UserWidget.h"
#include "PauseWidget.generated.h"

class UButton;
class USettingsWidget;

DECLARE_DELEGATE(FOnResumeRequested);

UCLASS()
class CR4S_API UPauseWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnResumeRequested OnResumeRequested;

protected:
	UFUNCTION()
	void OnResumeButtonClicked();
	UFUNCTION()
	void OnSettingsButtonClicked();
	UFUNCTION()
	void OnToMenuButtonClicked();

	UPROPERTY(meta = (BindWidget))
	UButton* ResumeButton;
	UPROPERTY(meta = (BindWidget))
	UButton* SettingsButton;
	UPROPERTY(meta = (BindWidget))
	UButton* ToMenuButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<USettingsWidget> SettingsWidgetClass;

	UPROPERTY()
	USettingsWidget* SettingsWidgetInstance;

	virtual void NativeConstruct() override;
};
