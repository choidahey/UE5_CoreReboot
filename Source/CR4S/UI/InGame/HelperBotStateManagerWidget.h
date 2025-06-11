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

protected:
	virtual void NativeConstruct() override;

	UPROPERTY() TObjectPtr<ABaseHelperBot> HelperBot;
	
	UPROPERTY(meta=(BindWidget)) class UCanvasPanel* RootCanvas;
	UPROPERTY(meta=(BindWidget)) class UButton* SetIdleButton;
	UPROPERTY(meta=(BindWidget)) class UButton* SetFollowingButton;
	UPROPERTY(meta=(BindWidget)) class UButton* SetChopWoodButton;
	UPROPERTY(meta=(BindWidget)) class UButton* CloseButton;
	UPROPERTY(meta=(BindWidget)) class UButton* SetMiningButton;
	UPROPERTY(meta=(BindWidget)) class UButton* OpenInventoryButton;
	UPROPERTY(meta=(BindWidget)) class UButton* SetRepairingButton;
	
protected:
	UFUNCTION() void SetIdle();
	UFUNCTION() void SetFollowing();
	UFUNCTION() void SetChopWood();
	UFUNCTION() void CloseStateWidget();
	UFUNCTION() void SetMining();
	UFUNCTION() void OpenInventory();
	UFUNCTION() void SetRepairing();

private:
	class AHelperBotAIController* OwnerAIController;
};