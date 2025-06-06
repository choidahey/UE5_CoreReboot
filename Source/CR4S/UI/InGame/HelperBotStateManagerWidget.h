#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../FriendlyAI/HelperBotState.h"
#include "HelperBotStateManagerWidget.generated.h"

class ABaseHelperBot;

UCLASS()
class CR4S_API UHelperBotStateManagerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void InitializeWithController(class AHelperBotAIController* InController);
	void CloseWidgetAndResetInput();
	
	UPROPERTY()
	TObjectPtr<UHelperBotStateManagerWidget> StateUIInstance;
protected:
	virtual void NativeConstruct() override;

	UPROPERTY() TObjectPtr<ABaseHelperBot> HelperBot;
	
	UPROPERTY(meta=(BindWidget)) class UCanvasPanel* RootCanvas;
	UPROPERTY(meta=(BindWidget)) class UButton* Button1;
	UPROPERTY(meta=(BindWidget)) class UButton* Button2;
	UPROPERTY(meta=(BindWidget)) class UButton* Button3;
	UPROPERTY(meta=(BindWidget)) class UButton* Button4;
	UPROPERTY(meta=(BindWidget)) class UButton* Button5;
	UPROPERTY(meta=(BindWidget)) class UButton* OpenInventoryButton;
	
protected:
	UFUNCTION() void OnButton1Clicked();
	UFUNCTION() void OnButton2Clicked();
	UFUNCTION() void OnButton3Clicked();
	UFUNCTION() void OnButton4Clicked();
	UFUNCTION() void OnButton5Clicked();
	UFUNCTION(BlueprintCallable) void OpenInventory();

private:
	class AHelperBotAIController* OwnerAIController;
};