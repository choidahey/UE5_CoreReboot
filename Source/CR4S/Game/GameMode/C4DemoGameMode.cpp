#include "Game/GameMode/C4DemoGameMode.h"
#include "Game/System/ObjectiveManager.h"

void AC4DemoGameMode::StartPlay()
{
    Super::StartPlay();

    ObjectiveManager = GetWorld()->GetSubsystem<UObjectiveManager>();
}