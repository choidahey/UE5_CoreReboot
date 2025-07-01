#pragma once

#include "GameFramework/GameModeBase.h"
#include "C4SurvivalGameMode.generated.h"

UCLASS()
class CR4S_API AC4SurvivalGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void ReturnToMenuWithDelay(float Delay);

	void ReturnToMenu();

protected:

	UFUNCTION(BlueprintCallable)
	void EndGameSequence();

	virtual void StartPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundBase* GameStartSFX;

	UPROPERTY()
	bool bIsTransitioningLevel = false;
private:

	void HandleStartGame();

};
