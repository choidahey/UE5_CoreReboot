#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HelperBotInfoWidget.generated.h"

class UTextBlock;
class UProgressBar;
class ABaseHelperBot;

UCLASS()
class CR4S_API UHelperBotInfoWidget : public UUserWidget
{
	GENERATED_BODY()
   
#pragma region Initilaize
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Init")
	void InitWidget(ABaseHelperBot* HelperBot);
   
#pragma endregion 
   
#pragma region BindWidget
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ProgressBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentHPTextBlock;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MaxHPTextBlock;
#pragma endregion

#pragma region BindingFunctions
public:
	UFUNCTION(BlueprintCallable)
	FText GetCurrentHealthText();
   
	UFUNCTION(BlueprintCallable)
	FText GetMaxHealthText();
   
	UFUNCTION(BlueprintCallable)
	float GetHealthPercent();

	UFUNCTION(BlueprintCallable)
	void SetOwnerHelperBot(ABaseHelperBot* HelperBot) { OwnerHelperBot = HelperBot; }

private:
	UPROPERTY()
	TObjectPtr<ABaseHelperBot> OwnerHelperBot = nullptr;
#pragma endregion

public:
	virtual void NativeConstruct() override;
	void UpdateWidgets();

private:
	FTimerHandle UpdateTimer;
};