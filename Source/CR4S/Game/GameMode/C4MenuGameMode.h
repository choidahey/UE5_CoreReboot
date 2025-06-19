#pragma once

#include "GameFramework/GameModeBase.h"
#include "C4MenuGameMode.generated.h"

UCLASS()
class CR4S_API AC4MenuGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	void OpenSurvivalLevel(int32 SlotIndex);

protected:
	virtual void BeginPlay() override;


private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> MainMenuWidgetClass;

	UUserWidget* MainMenuWidget;
};
