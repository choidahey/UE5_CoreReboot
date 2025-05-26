#include "MonsterAI/Task/Season/BTTask_SelectSkill_Hiems.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "MonsterAI/MonsterAIHelper.h"
#include "MonsterAI/Controller/BaseMonsterAIController.h"

int32 UBTTask_SelectSkill_Hiems::SelectSkillFromAvailable(const TArray<int32>& AvailableSkills, AActor* Target)
{
	if (!CachedMonster.IsValid() || !Target) return INDEX_NONE;

    constexpr float IceRoadForwardTreshold = 1100.f;
    constexpr float IceRaodAwayTreshold = 500.f;
    
    ABaseMonsterAIController* AICon = Cast<ABaseMonsterAIController>(CachedMonster->GetController());
    if (!AICon)
        return INDEX_NONE;

    UBlackboardComponent* BB = AICon->GetBlackboardComponent();

    const float Distance = FVector::Dist(CachedMonster->GetActorLocation(), Target->GetActorLocation());

    if (Distance >= IceRoadForwardTreshold || Distance <= IceRaodAwayTreshold)
    {
        const bool bToward = (Distance >= IceRoadForwardTreshold);
        BB->SetValueAsBool(FSeasonBossAIKeys::bIsIceRoadForward, bToward);

        return 3;
    }
	if (!CachedMonster.IsValid()) return INDEX_NONE;

    const FVector MonsterLocation = CachedMonster->GetActorLocation();
    float Distance = TNumericLimits<float>::Max();

    if (Target)
    {
        Distance = FVector::Dist(MonsterLocation, Target->GetActorLocation());
    }
    else
    {
        if (AAIController* AIC = Cast<AAIController>(CachedMonster->GetController()))
        {
            if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
            {
                AActor* HouseActor = Cast<AActor>(BB->GetValueAsObject(FSeasonBossAIKeys::NearestHouseActor));
                FVector HouseLocation = HouseActor->GetActorLocation();
                if (!HouseLocation.IsNearlyZero())
                {
                    Distance = FVector::Dist(MonsterLocation, HouseLocation);
                }
                else return INDEX_NONE;
            }
        }
    }
	
	TArray<FSkillWeight> Weights;
	Weights.Reserve(AvailableSkills.Num());

    for (int32 SkillID : AvailableSkills)
    {
        int32 Weight = 1;

        if (Distance >= 1100.f && (SkillID == 3 || SkillID == 5))
        {
            Weight = 5;
        }
        else if (Distance >= 500.f && Distance < 1100.f && SkillID == 2)
        {
            Weight = 5;
        }
        else if (Distance < 500.f && (SkillID == 5 || SkillID == 3))
        {
            Weight = 5;
        }

        Weights.Add({ SkillID, Weight });
    }

    // Save Weight
    int32 TotalWeight = 0;
    for (const FSkillWeight& SkillWeight : Weights)
    {
        TotalWeight += SkillWeight.Weight;
    }
    if (TotalWeight <= 0)
        return INDEX_NONE;

    // return random skill index
    int32 RandomIndex = FMath::RandRange(1, TotalWeight);
    for (const FSkillWeight& SkillWeight : Weights)
    {
        RandomIndex -= SkillWeight.Weight;
        if (RandomIndex <= 0)
        {
            //return SkillWeight.SkillID;
            return 2;
        }
    }

    return INDEX_NONE;
}
