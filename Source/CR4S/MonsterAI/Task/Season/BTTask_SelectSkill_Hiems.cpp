#include "MonsterAI/Task/Season/BTTask_SelectSkill_Hiems.h"
#include "MonsterAI/Controller/BaseMonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"

UBTTask_SelectSkill_Hiems::UBTTask_SelectSkill_Hiems()
{
    NodeName = "SelectSkill_Hiems";
    bCreateNodeInstance = true;
    CurrentShuffleIndex = 0;
}

int32 UBTTask_SelectSkill_Hiems::SelectSkillFromAvailable(const TArray<int32>& AvailableSkills, AActor* Target)
{
    if (!CachedMonster.IsValid()) return INDEX_NONE;
    
    ABaseMonsterAIController* AIC = Cast<ABaseMonsterAIController>(CachedMonster->GetController());
    if (!IsValid(AIC)) return INDEX_NONE;

    UBlackboardComponent* BB = AIC->GetBlackboardComponent();
    if (!IsValid(BB)) return INDEX_NONE;

    if (IsValid(Target))
    {
        const FVector MonsterLoc = CachedMonster->GetActorLocation();
        const FVector TargetLoc  = Target->GetActorLocation();
        const float Distance     = FVector::Dist(MonsterLoc, TargetLoc);
        
        if (Distance <= IceRoadAwayTreshold || Distance >= IceRoadForwardTreshold)
        {
            const bool bToward = (Distance >= IceRoadForwardTreshold);
            BB->SetValueAsBool(FSeasonBossAIKeys::bIsIceRoadForward, bToward);

            return 3;
        }
    }
    
    if (AvailableSkills.Num() == 0) return INDEX_NONE;
    
    TArray<int32> FilteredSkills = AvailableSkills;
    FilteredSkills.Remove(3);

    if (FilteredSkills.Num() == 0) return INDEX_NONE;
    
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

void UBTTask_SelectSkill_Hiems::ReshuffleSkills(const TArray<int32>& AvailableSkills)
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
