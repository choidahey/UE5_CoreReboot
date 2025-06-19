#pragma once

#include "GameFramework/GameModeBase.h"
#include "C4SurvivalGameMode.generated.h"

UCLASS()
class CR4S_API AC4SurvivalGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	void ReturnToMenu();

protected:
	UFUNCTION(BlueprintCallable)
	void EndGameSequence();
	virtual void StartPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundBase* GameStartSFX;

private:

	void HandleStartGame();
};
