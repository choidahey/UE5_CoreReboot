#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "../UI/InGame/HelperBotStateManagerWidget.h"
#include "LIHCheatPlayerController.generated.h"

class UHelperBotStateManagerWidget;

UCLASS()
class CR4S_API ALIHCheatPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ALIHCheatPlayerController();
	
	UFUNCTION(Exec)
	void CMD_SpawnAnimal(const FString& AnimalBPPath, int32 Count = 1);
	
	UFUNCTION(Exec)
	void CMD_DamageAll(float DamageAmount);
	
	UFUNCTION(Exec)
	void CMD_StunAll(float StunAmount);
	
	UFUNCTION(Exec)
	void CMD_KillAll();

	// UFUNCTION(Exec)
	// void CMD_ImpregnateAll();

	UFUNCTION(Exec)
	void CMD_ChangeHelperBotState(EHelperBotState NewState);
	
	UPROPERTY(EditDefaultsOnly, Category = "Cheat")
	TSubclassOf<UHelperBotStateManagerWidget> CheatWidgetClass;

	UHelperBotStateManagerWidget* CheatWidgetInstance;

	UFUNCTION(Exec)
	void CMD_ToggleHelperBotUI();
};