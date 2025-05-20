#include "Game/CheatManager/C4CheatManager.h"
#include "Game/CheatManager/TimeCheatHelper.h"

void UC4CheatManager::InitCheatManager()
{
    Super::InitCheatManager();

    //Create Helper and Bind
    TimeHelper = NewObject<UTimeCheatHelper>(this);
}

void UC4CheatManager::AddMinute(int32 Amount)
{
    if (TimeHelper)
    {
        //Acutal Logic for the Cheat Command processed in Helper
        TimeHelper->AddMinute(Amount);
    }
}

void UC4CheatManager::AddDay(int32 Amount)
{
    if (TimeHelper)
    {
        TimeHelper->AddDay(Amount);
    }
}
