#pragma once

#include "GameFramework/GameMode.h"
#include "C4CineGameMode.generated.h"

class ULoadingWidget;

UCLASS()
class CR4S_API AC4CineGameMode : public AGameMode
{
	GENERATED_BODY()
	
protected:

	virtual void StartPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<ULoadingWidget> LoadingWidgetClass;

	UFUNCTION()
	void GoToMainMap();

	UFUNCTION(BlueprintCallable)
	void PlayCinematic();

	UFUNCTION(BlueprintCallable)
	void NotifyCinematicFinished();
};
