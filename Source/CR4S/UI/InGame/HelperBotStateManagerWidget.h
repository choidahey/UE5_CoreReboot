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
	void InitializeWithController(AHelperBotAIController* InController, EHelperBotState InPreviousState);
	void CloseWidgetAndResetInput();
	void CloseWidgetAndRestorePreviousState();
	virtual void NativeConstruct() override;
protected:
	UFUNCTION() void SetIdle();
	UFUNCTION() void SetFollowing();
	UFUNCTION() void SetChopWood();
	UFUNCTION() void CloseStateWidget();
	UFUNCTION() void SetMining();
	UFUNCTION() void SetDefending();
	UFUNCTION() void OpenInventory();
	UFUNCTION() void SetRepairing();
	UFUNCTION() void UpdateLookAtPlayer();
	UFUNCTION() void CheckPlayerDistance();
	
protected:
	UPROPERTY(meta=(BindWidget)) class UCanvasPanel* RootCanvas;
	UPROPERTY(meta=(BindWidget)) class UButton* SetIdleButton;
	UPROPERTY(meta=(BindWidget)) class UButton* SetFollowingButton;
	UPROPERTY(meta=(BindWidget)) class UButton* SetChopWoodButton;
	UPROPERTY(meta=(BindWidget)) class UButton* CloseButton;
	UPROPERTY(meta=(BindWidget)) class UButton* SetMiningButton;
	UPROPERTY(meta=(BindWidget)) class UButton* OpenInventoryButton;
	UPROPERTY(meta=(BindWidget)) class UButton* SetRepairingButton;
	UPROPERTY(meta=(BindWidget)) class UButton* SetDefendingButton;

	UPROPERTY() TObjectPtr<ABaseHelperBot> HelperBot = nullptr;

private:
	UPROPERTY()
	class AHelperBotAIController* OwnerAIController = nullptr;
	
	FRotator TargetLookRotation;
	
	UPROPERTY() float MaxInteractionDistance = 300.0f;
	UPROPERTY() EHelperBotState PreviousState;
	UPROPERTY() FTimerHandle DistanceCheckTimer;
	UPROPERTY() FTimerHandle LookAtPlayerTimer;
};