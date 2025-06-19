#pragma once

#include "GameFramework/GameModeBase.h"
#include "C4MenuGameMode.generated.h"

class UMainMenuWidget;
class ULoadingWidget;


UCLASS()
class CR4S_API AC4MenuGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	void OpenSurvivalLevel(int32 SlotIndex);

	

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UMainMenuWidget> MainMenuWidgetClass;

	UPROPERTY()
	TObjectPtr<UMainMenuWidget> MainMenuWidgetInstance;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<ULoadingWidget> LoadingWidgetClass;

private:

	void LoadSurvivalLevel();



};
