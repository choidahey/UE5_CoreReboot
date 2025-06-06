#include "MonsterAI/Task/Season/BTTask_SelectSkill_Ceres.h"
#include "MonsterAI/Controller/BaseMonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MonsterAI/BaseMonster.h"

UBTTask_SelectSkill_Ceres::UBTTask_SelectSkill_Ceres()
{
    NodeName = "SelectSkill_Ceres";
    bCreateNodeInstance = true;
    CurrentShuffleIndex = 0;
}

int32 UBTTask_SelectSkill_Ceres::SelectSkillFromAvailable(const TArray<int32>& AvailableSkills, AActor* Target)
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
    
    // return ChosenSkillID;
    return 1;
}

void UBTTask_SelectSkill_Ceres::ReshuffleSkills(const TArray<int32>& AvailableSkills)
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
