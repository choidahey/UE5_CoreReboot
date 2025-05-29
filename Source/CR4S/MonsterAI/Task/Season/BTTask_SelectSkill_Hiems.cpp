#include "MonsterAI/Task/Season/BTTask_SelectSkill_Hiems.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "MonsterAI/MonsterAIHelper.h"
#include "MonsterAI/Controller/BaseMonsterAIController.h"

UBTTask_SelectSkill_Hiems::UBTTask_SelectSkill_Hiems()
{
    NodeName = "SelectSkill_Hiems";
    bCreateNodeInstance = true;
}

int32 UBTTask_SelectSkill_Hiems::SelectSkillFromAvailable(const TArray<int32>& AvailableSkills, AActor* Target)
{
	if (!CachedMonster.IsValid()) return INDEX_NONE;
    
    ABaseMonsterAIController* AICon = Cast<ABaseMonsterAIController>(CachedMonster->GetController());
    if (!AICon)
        return INDEX_NONE;

    UBlackboardComponent* BB = AICon->GetBlackboardComponent();

    float Distance = 0.f;
    if (Target)
    {
        Distance = FVector::Dist(CachedMonster->GetActorLocation(), Target->GetActorLocation());
    }
    else
    {
        UObject* Obj = BB->GetValueAsObject(FSeasonBossAIKeys::NearestHouseActor);
        if (AActor* HouseActor = Cast<AActor>(Obj))
        {
            Distance = FVector::Dist(CachedMonster->GetActorLocation(), HouseActor->GetActorLocation());
        }
        else
        {
            return INDEX_NONE;
        }
    }

    // TODO :: 이 부분 태스크로 뺄 것
    // if (Distance >= IceRoadForwardTreshold || Distance <= IceRoadAwayTreshold)
    // {
    //     const bool bToward = (Distance >= IceRoadForwardTreshold);
    //     BB->SetValueAsBool(FSeasonBossAIKeys::bIsIceRoadForward, bToward);
    //
    //     return 3;
    // }
	
	TArray<FSkillWeight> Weights;
	Weights.Reserve(AvailableSkills.Num());

    for (int32 SkillID : AvailableSkills)
    {
        int32 Weight = 1;

        if (Distance >= IceRoadForwardTreshold && SkillID == 5)
        {
            Weight = 5;
        }
        else if (Distance >= IceRoadAwayTreshold && Distance < IceRoadForwardTreshold && SkillID == 2)
        {
            Weight = 5;
        }
        else if (Distance < IceRoadAwayTreshold && SkillID == 5)
        {
            Weight = 5;
        }

        Weights.Add({ SkillID, Weight });
    }
    
    int32 TotalWeight = 0;
    for (const FSkillWeight& SkillWeight : Weights)
    {
        TotalWeight += SkillWeight.Weight;
    }
    if (TotalWeight <= 0)
        return INDEX_NONE;
    
    int32 RandomIndex = FMath::RandRange(1, TotalWeight);
    for (const FSkillWeight& SkillWeight : Weights)
    {
        RandomIndex -= SkillWeight.Weight;
        if (RandomIndex <= 0)
        {
            return SkillWeight.SkillID;
            // return 2;
        }
    }

    return INDEX_NONE;
}
