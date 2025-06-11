#include "Game/CheatManager/C4CheatManager.h"

#include "EngineUtils.h"
#include "Game/CheatManager/TimeCheatHelper.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/Components/MonsterAttributeComponent.h"
#include "CR4S.h"

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

void UC4CheatManager::SetMonstersHP(const float InHealth)
{
    if (UWorld* World = GetWorld())
    {
        int32 Count = 0;
        for (TActorIterator<ABaseMonster> It(World); It; ++It)
        {
            if (ABaseMonster* Monster = *It)
                if (auto* Attr = Monster->FindComponentByClass<UMonsterAttributeComponent>())
                {
                    Attr->SetCurrentHP(InHealth);
                    ++Count;

                    if (Attr->GetCurrentHP() <= 0.f)
                    {
                        Monster->HandleDeath(); 
                    }
                }
        }
        CR4S_Log(LogDa, Log, TEXT("[%s] %d monsters' health set to %.1f"), *GetClass()->GetName(), Count, InHealth);
    }
}
