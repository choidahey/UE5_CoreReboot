#include "Game/CheatManager/C4CheatManager.h"
#include "EngineUtils.h"
#include "Game/CheatManager/TimeCheatHelper.h"
#include "Game/CheatManager/SaveGameHelper.h"
#include "CR4S.h"
#include "ItemGimmickHelper.h"
#include "MonsterAI/MonsterAIHelper.h"

void UC4CheatManager::InitCheatManager()
{
    Super::InitCheatManager();

    //Create Helper and Bind
    TimeHelper = NewObject<UTimeCheatHelper>(this);

    ItemGimmickHelper = NewObject<UItemGimmickHelper>(this);
    SaveGameHelper = NewObject<USaveGameHelper>(this);
}

void UC4CheatManager::SaveNow()
{
    if (SaveGameHelper)
    {
        SaveGameHelper->SaveNow();
    }
}

void UC4CheatManager::AddItem(const FName RowName, const int32 Count) const
{
    if (CR4S_VALIDATE(LogCheatManager, IsValid(ItemGimmickHelper)))
    {
        ItemGimmickHelper->AddItem(RowName, Count);
    }
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

void UC4CheatManager::SetMonstersHP(const float InHealth)
{
    MonsterAIHelper::SetMonstersHP(GetWorld(), InHealth);
}
