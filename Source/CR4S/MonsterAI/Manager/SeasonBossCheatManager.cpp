#include "MonsterAI/Manager/SeasonBossCheatManager.h"
#include "GameFramework/PlayerController.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/Components/MonsterAttributeComponent.h"
#include "EngineUtils.h"

void USeasonBossCheatManager::SetSeasonBossHealth(const FString& MonsterID, float NewHealth)
{
    UWorld* W = GetWorld();
    if (!W) return;

    int32 Found = 0;
    for (TActorIterator<ABaseMonster> It(W); It; ++It)
    {
        ABaseMonster* M = *It;
        if (M->GetName().Contains(MonsterID))
        {
            if (auto* AttrComp = M->FindComponentByClass<UMonsterAttributeComponent>())
            {
                AttrComp->SetCurrentHP(NewHealth);
                ++Found;
            }
        }
    }
    UE_LOG(LogTemp, Log, TEXT("[MonsterCheatManager] Set %s HP : %.2f"), *MonsterID, NewHealth);
}

void USeasonBossCheatManager::HealSeasonBossHealth(const FString& MonsterID, float HealAmount)
{
    UWorld* W = GetWorld();
    if (!W) return;

    int32 Count = 0;
    for (TActorIterator<ABaseMonster> It(W); It; ++It)
    {
        ABaseMonster* M = *It;
        if (M->GetName().Contains(MonsterID))
            if (auto* AttrComp = M->FindComponentByClass<UMonsterAttributeComponent>())
            {
                AttrComp->SetCurrentHP(AttrComp->GetCurrentHP() + HealAmount);
                ++Count;
            }
    }
    UE_LOG(LogTemp, Log, TEXT("[MonsterCheatManager] Heal %s : %.2f"), *MonsterID, HealAmount);
}

void USeasonBossCheatManager::DamageSeasonBossHealth(const FString& MonsterID, float DamageAmount)
{
    UWorld* W = GetWorld();
    if (!W) return;

    int32 Count = 0;
    for (TActorIterator<ABaseMonster> It(W); It; ++It)
    {
        ABaseMonster* M = *It;
        if (M->GetName().Contains(MonsterID))
            if (auto* AttrComp = M->FindComponentByClass<UMonsterAttributeComponent>())
            {
                AttrComp->SetCurrentHP(AttrComp->GetCurrentHP() - DamageAmount);
                ++Count;
            }
    }
    UE_LOG(LogTemp, Log, TEXT("[MonsterCheatManager] Damaged %s : %.2f"), *MonsterID, DamageAmount);
}

void USeasonBossCheatManager::KillMonster(const FString& MonsterID)
{
    UWorld* World = GetWorld();
    if (!World) return;

    int32 Count = 0;
    for (TActorIterator<ABaseMonster> It(World); It; ++It)
    {
        ABaseMonster* Monster = *It;
        if (Monster->GetName().Contains(MonsterID))
            if (auto* AttrComp = Monster->FindComponentByClass<UMonsterAttributeComponent>())
            {
                AttrComp->SetCurrentHP(0.f);
                ++Count;
            }
    }
    UE_LOG(LogTemp, Log, TEXT("[MonsterCheatManager] Kill %s"), *MonsterID);
}
