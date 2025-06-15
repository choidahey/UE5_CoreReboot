#include "MonsterAI/Task/Season/BTTask_SelectSkill_Vita.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/Controller/BaseMonsterAIController.h"

UBTTask_SelectSkill_Vita::UBTTask_SelectSkill_Vita()
{
    NodeName = "SelectSkill_Vita";
    bCreateNodeInstance = true;
    CurrentShuffleIndex = 0;
}

int32 UBTTask_SelectSkill_Vita::SelectSkillFromAvailable(const TArray<int32>& AvailableSkills, AActor* Target)
{
    if (!CachedMonster.IsValid()) return INDEX_NONE;
    
    ABaseMonsterAIController* AIC = Cast<ABaseMonsterAIController>(CachedMonster->GetController());
    if (!IsValid(AIC)) return INDEX_NONE;

    UBlackboardComponent* BB = AIC->GetBlackboardComponent();
    if (!IsValid(BB)) return INDEX_NONE;

    if (AvailableSkills.Num() == 0)
    {
        return INDEX_NONE;
    }

    bool bNeedReshuffle = false;
    
    if (ShuffledSkills.Num() != AvailableSkills.Num())
    {
        bNeedReshuffle = true;
    }
    else
    {
        if (ShuffledSkills.Num() == 0 || CurrentShuffleIndex >= ShuffledSkills.Num())
        {
            bNeedReshuffle = true;
        }
        else
        {
            bool bSame = true;
            for (int32 ID : AvailableSkills)
            {
                if (!ShuffledSkills.Contains(ID))
                {
                    bSame = false;
                    break;
                }
            }
            if (!bSame)
            {
                bNeedReshuffle = true;
            }
        }
    }

    if (bNeedReshuffle)
    {
        ReshuffleSkills(AvailableSkills);
    }
    
    if (CurrentShuffleIndex < 0 || CurrentShuffleIndex >= ShuffledSkills.Num())
    {
        ReshuffleSkills(AvailableSkills);
    }
    
    int32 ChosenSkillID = ShuffledSkills[CurrentShuffleIndex];
    CurrentShuffleIndex++;
    
    return ChosenSkillID;
}

void UBTTask_SelectSkill_Vita::ReshuffleSkills(const TArray<int32>& AvailableSkills)
{
    ShuffledSkills = AvailableSkills;

    for (int32 i = ShuffledSkills.Num() - 1; i > 0; --i)
    {
        int32 j = FMath::RandRange(0, i);
        if (i != j)
        {
            ShuffledSkills.Swap(i, j);
        }
    }

    CurrentShuffleIndex = 0;
}