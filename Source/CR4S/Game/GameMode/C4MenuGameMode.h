#pragma once

#include "GameFramework/GameModeBase.h"
#include "C4MenuGameMode.generated.h"

UCLASS()
class CR4S_API AC4MenuGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:

protected:
	virtual void BeginPlay() override;


private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> MainMenuWidgetClass;

	UUserWidget* MainMenuWidget;
};
