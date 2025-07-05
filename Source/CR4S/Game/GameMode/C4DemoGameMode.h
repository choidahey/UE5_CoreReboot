#pragma once

#include "GameFramework/GameModeBase.h"
#include "C4DemoGameMode.generated.h"

class UObjectiveManager;

UCLASS()
class CR4S_API AC4DemoGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	virtual void StartPlay() override;

	TObjectPtr<UObjectiveManager> ObjectiveManager;

};
