#include "Game/CheatManager/TimeCheatHelper.h"
#include "Game/System/WorldTimeManager.h"
#include "Kismet/GameplayStatics.h"

void UTimeCheatHelper::AddMinute(int32 Amount)
{
    if (UWorld* World = GetWorld())
    {
        if (UWorldTimeManager* TimeManager = World->GetSubsystem<UWorldTimeManager>())
        {
            TimeManager->ModifyTime(0, Amount); // example: ModifyTime(Day, Minute, Second)
            UE_LOG(LogTemp, Warning, TEXT("Added %d minutes"), Amount);
        }
    }
}

void UTimeCheatHelper::AddDay(int32 Amount)
{
    if (UWorld* World = GetWorld())
    {
        if (UWorldTimeManager* TimeManager = World->GetSubsystem<UWorldTimeManager>())
        {
            TimeManager->ModifyTime(Amount, 0);
            UE_LOG(LogTemp, Warning, TEXT("Added %d days"), Amount);
        }
    }
}