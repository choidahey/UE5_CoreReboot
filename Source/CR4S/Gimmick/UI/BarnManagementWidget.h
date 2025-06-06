#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BarnManagementWidget.generated.h"

UCLASS()
class CR4S_API UBarnManagementWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "BarnManagement")
	void RefreshBarnUI();

	UPROPERTY()
	TObjectPtr<class ABarnGimmick> OwningGimmick;

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> ExitButton;

	UFUNCTION()
	void OnExitClicked();

	UFUNCTION()
	void OnTestButtonClicked();

	UFUNCTION()
	void OnTestButton2Clicked();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> TestButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> TestButton2;

};
