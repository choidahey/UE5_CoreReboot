#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HelperBotManagementWidget.generated.h"

class ABaseHelperBot;
class UButton;
class UHelperBotRepairWidget;

UCLASS()
class CR4S_API UHelperBotManagementWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region Initialize

public:
	void InitWidget(UHelperBotRepairWidget* NewHelperBotRepairWidget);

	FORCEINLINE void SetTargetHelperBot(ABaseHelperBot* NewTargetHelperBot) { TargetHelperBot = NewTargetHelperBot; }

private:
	UPROPERTY()
	TObjectPtr<UHelperBotRepairWidget> HelperBotRepairWidget;
	
	UPROPERTY()
	TObjectPtr<ABaseHelperBot> TargetHelperBot;

#pragma endregion

#pragma region ToggleWidget

public:
	UFUNCTION()
	void OpenWidget();
	void CloseWidget();
	
#pragma endregion 

#pragma region HelperBot Repair

private:
	UFUNCTION()
	void OpenHelperBotRepairWidget();
	
#pragma endregion 

#pragma region BindWidget

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BotRepairButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> FollowingButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> StopButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ChopTreeButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MiningButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> DefendingButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RepairingButton;

#pragma endregion
};
