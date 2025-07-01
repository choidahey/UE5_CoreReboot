#include "MonsterAI/Task/Season/BTTask_SelectSkill_Hiems.h"

#include "CR4S.h"
#include "MonsterAI/Controller/BaseMonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"


// TODO :: 이넘 클래스 가져와서 블랙보드에 있는 AttackType 가져와서 랜덤 스킬 추출
UBTTask_SelectSkill_Hiems::UBTTask_SelectSkill_Hiems()
{
    NodeName = "SelectSkill_Hiems";
    bCreateNodeInstance = true;
    CurrentShuffleIndex = 0;
}

int32 UBTTask_SelectSkill_Hiems::SelectSkillFromAvailable(const TArray<int32>& AvailableSkills, AActor* Target)
{
    if (!CachedMonster.IsValid() || AvailableSkills.Num() == 0)
        return INDEX_NONE;
    
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    float DistanceToPlayer = PlayerPawn ? FVector::Dist(CachedMonster->GetActorLocation(), PlayerPawn->GetActorLocation()) : FLT_MAX;

    CR4S_Log(LogDa, Log, TEXT("[%s] DistanceToPlayer : %f"), *GetClass()->GetName(), DistanceToPlayer);

    TMap<int32, float> SkillWeights;
    const float NearDist = 1500.f;  // 임시로 대시랑 안겹치기 위해 대시 거리에서 500씩 더하고 빼줌
    const float FarDist = 4000.f;

    // 가중치 계산
    for (int32 SkillID : AvailableSkills)
    {
        float Weight = 0.f;
        switch (static_cast<EHiemsSkill>(SkillID))
        {
            // near skill - blizzard(0), icicleWave(1), iceFlower(6)
            case EHiemsSkill::Blizzard:
            case EHiemsSkill::IcicleWave:
            case EHiemsSkill::IceFlower:
                Weight = FMath::Clamp((NearDist - DistanceToPlayer) / NearDist, 0.f, 1.f);
                break;

            // far skill - iceSpike(2), coldFairy(3), iceRain(7)
            case EHiemsSkill::IceSpike:
            case EHiemsSkill::ColdFairy:
            case EHiemsSkill::IceRain:
                if (DistanceToPlayer >= FarDist)
                {
                    Weight = 1.f;
                }
                else if (DistanceToPlayer > NearDist)
                {
                    Weight = (DistanceToPlayer - NearDist) / (FarDist - NearDist);
                }
                else
                {
                    Weight = 0.f;
                }
                break;

            // moving skill - IceRoadAway(4), IceRoadForward(5)
            case EHiemsSkill::IceRoadAway:
                Weight = (DistanceToPlayer < DistanceAwayIceRoad) ? 1.f : 0.f;
                break;

            case EHiemsSkill::IceRoadForward:
                Weight = (DistanceToPlayer > DistanceForwordIceRoad) ? 1.f : 0.f;
                break;

            default:
                Weight = 0.f;
        }
        SkillWeights.Add(SkillID, Weight);
    }

    TArray<TPair<int32, float>> ValidSkills;
    for (TPair<int32, float>& Pair : SkillWeights)
    {
        if (Pair.Value > 0.f)
        {
            ValidSkills.Add(Pair);
        }
    }

    // 가중치가 있는 스킬이 없으면 폴백 로직
    if (ValidSkills.Num() == 0)
    {
        TArray<int32> FallbackSkills;
        for (int32 SkillID : AvailableSkills)
        {
            EHiemsSkill Skill = static_cast<EHiemsSkill>(SkillID);
            if (Skill != EHiemsSkill::IceRoadAway && Skill != EHiemsSkill::IceRoadForward)
            {
                FallbackSkills.Add(SkillID);
            }
        }

        if (FallbackSkills.Num() > 0)
        {
            ReshuffleSkills(FallbackSkills);
            int32 SelectedSkill = ShuffledSkills.IsValidIndex(CurrentShuffleIndex) ? ShuffledSkills[CurrentShuffleIndex++] : INDEX_NONE;
            PreviousSkillIndex = SelectedSkill;
            return SelectedSkill;
        }
        return INDEX_NONE;
    }

    // 가중치를 기준으로 정렬 (높은 가중치 순)
    ValidSkills.Sort([](const TPair<int32, float>& A, const TPair<int32, float>& B) {
        return A.Value > B.Value;
    });

    int32 SelectedSkill = INDEX_NONE;

    
    float MaxWeight = ValidSkills[0].Value;
    TArray<int32> TopSkills;
    
    for (const TPair<int32, float>& Pair : ValidSkills)
    {
        if (FMath::IsNearlyEqual(Pair.Value, MaxWeight, 0.01f))
        {
            TopSkills.Add(Pair.Key);
        }
        else
        {
            break; // 이미 정렬되어 있으므로 가중치가 다르면 중단
        }
    }
    
    // 이전 스킬이 최고 가중치 그룹에 있다면 제거
    if (TopSkills.Contains(PreviousSkillIndex))
    {
        TopSkills.Remove(PreviousSkillIndex);
    }
    
    // 남은 스킬이 있으면 그 중에서 랜덤 선택
    if (TopSkills.Num() > 0)
    {
        int32 RandomIndex = FMath::RandRange(0, TopSkills.Num() - 1);
        SelectedSkill = TopSkills[RandomIndex];
    }
    // 남은 스킬이 없으면 (모든 최고 가중치 스킬이 이전 스킬과 같음) 두 번째 가중치 그룹에서 선택
    else if (ValidSkills.Num() > 1)
    {
        // 두 번째 가중치 값 찾기
        float SecondWeight = -1.f;
        for (int32 i = 1; i < ValidSkills.Num(); ++i)
        {
            if (!FMath::IsNearlyEqual(ValidSkills[i].Value, MaxWeight, 0.01f))
            {
                SecondWeight = ValidSkills[i].Value;
                break;
            }
        }
        
        if (SecondWeight > 0.f)
        {
            TArray<int32> SecondTopSkills;
            for (const TPair<int32, float>& Pair : ValidSkills)
            {
                if (FMath::IsNearlyEqual(Pair.Value, SecondWeight, 0.01f))
                {
                    SecondTopSkills.Add(Pair.Key);
                }
            }
            
            if (SecondTopSkills.Num() > 0)
            {
                int32 RandomIndex = FMath::RandRange(0, SecondTopSkills.Num() - 1);
                SelectedSkill = SecondTopSkills[RandomIndex];
            }
        }
    }
    
    // 그래도 선택된 스킬이 없으면 원래 최고 가중치 스킬 사용 (중복 허용)
    if (SelectedSkill == INDEX_NONE && ValidSkills.Num() > 0)
    {
        SelectedSkill = ValidSkills[0].Key;
    }

    PreviousSkillIndex = SelectedSkill;
    return SelectedSkill;
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
