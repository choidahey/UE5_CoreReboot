#include "BTService_SelectAttackType.h"
#include "AIController.h"
#include "CR4S.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "MonsterAI/Data/MonsterSkillData.h"

UBTService_SelectAttackType::UBTService_SelectAttackType()
{
	NodeName = TEXT("Select AttackType");
	bNotifyTick = true;
	AttackTypeKey.SelectedKeyName = FSeasonBossAIKeys::AttackType;
	CurrentDistanceKey.SelectedKeyName = FSeasonBossAIKeys::CurrentDistance;
	MeleeOptimalDistanceKey.SelectedKeyName = FSeasonBossAIKeys::MeleeOptimalDistance;
	RangedOptimalDistanceKey.SelectedKeyName = FSeasonBossAIKeys::RangedOptimalDistance;
	CurrentPhaseKey.SelectedKeyName = NAME_None;  // 이건 아직 키 추가 안함
}

void UBTService_SelectAttackType::TickNode(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory,
	float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		if (bShowDebugInfo)
		{
			CR4S_Log(LogDa, Warning, TEXT("[%s] Missing BB"), *NodeName);
		}
		return;
	}
	
	const float Distance = BB->GetValueAsFloat(CurrentDistanceKey.SelectedKeyName);
	
	const ESkillAttackType Chosen = SelectSkillType(OwnerComp, Distance);
	BB->SetValueAsEnum(AttackTypeKey.SelectedKeyName, static_cast<uint8>(Chosen));
	
	BB->SetValueAsFloat(MeleeOptimalDistanceKey.SelectedKeyName, SkillTypeWeightConfig.MeleeOptimalDistance);
	BB->SetValueAsFloat(RangedOptimalDistanceKey.SelectedKeyName, SkillTypeWeightConfig.RangedOptimalDistance);
	
	RecordSkillUsage(OwnerComp, Chosen);

	if (bShowDebugInfo)
	{
		const FString TypeStr = (Chosen == ESkillAttackType::Melee) ? TEXT("Melee") : TEXT("Ranged");
		CR4S_Log(LogDa, Log, TEXT("[%s] Distance: %.1f → %s"), *NodeName, Distance, *TypeStr);
	}
}

ESkillAttackType UBTService_SelectAttackType::SelectSkillType(UBehaviorTreeComponent& OwnerComp, float Distance)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	const FSkillTypeWeightConfig& SkillWeightConfig = GetActiveConfig(BB);

	// 순수 거리 가중치 계산
	float RawMelee = SkillWeightConfig.BaseMeleeWeight;
	float RawRanged = SkillWeightConfig.BaseRangedWeight;
	ComputeRawDistanceWeights(Distance, SkillWeightConfig, RawMelee, RawRanged);

	// BaseRate 비중으로 블렌딩
	float MeleeW  = FMath::Lerp(SkillWeightConfig.BaseMeleeWeight, RawMelee,  BaseRate);
	float RangedW = FMath::Lerp(SkillWeightConfig.BaseRangedWeight, RawRanged, BaseRate);

	if (Distance > SkillWeightConfig.MeleeOptimalDistance)    MeleeW = 0.f;
	if (Distance <= SkillWeightConfig.RangedOptimalDistance)   RangedW = 0.f;

	// 연속 사용 방지
	if (SkillWeightConfig.bPreventConsecutive)
		EnforceConsecutiveRule(OwnerComp, SkillWeightConfig, MeleeW, RangedW);

	// 최소 가중치 보장
	NormalizeWeights(MeleeW, RangedW);

	const float Sum = MeleeW + RangedW;
	
	ESkillAttackType ChosenType;
	if (Sum <= KINDA_SMALL_NUMBER)
	{
		ChosenType = (SkillWeightConfig.BaseMeleeWeight >= SkillWeightConfig.BaseRangedWeight)
			? ESkillAttackType::Melee : ESkillAttackType::Ranged;
	}
	else
	{
		const bool bChooseMelee = (UKismetMathLibrary::RandomFloat() < MeleeW / Sum);
		ChosenType = bChooseMelee ? ESkillAttackType::Melee : ESkillAttackType::Ranged;
	}
	
	return ChosenType;
}

const FSkillTypeWeightConfig& UBTService_SelectAttackType::GetActiveConfig(const UBlackboardComponent* BB) const
{
	if (BB && CurrentPhaseKey.SelectedKeyName != NAME_None)
	{
		const int32 Phase = BB->GetValueAsInt(CurrentPhaseKey.SelectedKeyName);
		if (const FSkillTypeWeightConfig* Found = PhaseConfigs.Find(Phase))
			return *Found;
	}
	return SkillTypeWeightConfig;
}

void UBTService_SelectAttackType::ComputeRawDistanceWeights(float Distance, const FSkillTypeWeightConfig& Cfg,
	float& OutMelee, float& OutRanged) const
{
	const float Influence = Cfg.DistanceInfluenceStrength;
	if (Distance <= Cfg.MeleeOptimalDistance)
	{
		const float Bonus = (Cfg.MeleeOptimalDistance - Distance) / Cfg.MeleeOptimalDistance;
		OutMelee  += Bonus * 50.f * Influence;
		OutRanged *= 1.f - Bonus * 0.5f * Influence;
	}
	else if (Distance >= Cfg.RangedOptimalDistance)
	{
		const float Bonus = FMath::Min(1.f, (Distance - Cfg.RangedOptimalDistance) / Cfg.RangedOptimalDistance);
		OutRanged += Bonus * 50.f * Influence;
		OutMelee  *= 1.f - Bonus * 0.5f * Influence;
	}
	else
	{
		const float Alpha = (Distance - Cfg.MeleeOptimalDistance) / (Cfg.RangedOptimalDistance - Cfg.MeleeOptimalDistance);
		OutMelee  *= 1.f - Alpha * Influence;
		OutRanged *= 1.f - (1.f - Alpha) * Influence;
	}
}

void UBTService_SelectAttackType::EnforceConsecutiveRule(UBehaviorTreeComponent& OwnerComp,
	const FSkillTypeWeightConfig& Cfg, float& InOutMelee, float& InOutRanged)
{
	const auto* History = RecentSkillHistory.Find(&OwnerComp);
	if (!History || History->Num() < Cfg.MaxConsecutiveCount) return;

	int32 ConsecM = 0, ConsecR = 0;
	const int32 Start = History->Num() - Cfg.MaxConsecutiveCount;
	for (int32 i = History->Num() - 1; i >= Start; --i)
	{
		if ((*History)[i] == ESkillAttackType::Melee)  ++ConsecM; else break;
	}
	for (int32 i = History->Num() - 1; i >= Start; --i)
	{
		if ((*History)[i] == ESkillAttackType::Ranged) ++ConsecR; else break;
	}

	if (ConsecM >= Cfg.MaxConsecutiveCount)
	{
		InOutMelee  *= 1.f - Cfg.ConsecutivePenalty;
		InOutRanged *= 1.f + Cfg.ConsecutivePenalty;
	}
	else if (ConsecR >= Cfg.MaxConsecutiveCount)
	{
		InOutRanged *= 1.f - Cfg.ConsecutivePenalty;
		InOutMelee  *= 1.f + Cfg.ConsecutivePenalty;
	}
}

void UBTService_SelectAttackType::RecordSkillUsage(UBehaviorTreeComponent& OwnerComp, ESkillAttackType Used)
{
	TArray<ESkillAttackType>& Arr = RecentSkillHistory.FindOrAdd(&OwnerComp);
	Arr.Add(Used);
	const int32 MaxEntries = SkillTypeWeightConfig.MaxConsecutiveCount + 2;
	while (Arr.Num() > MaxEntries)
	{
		Arr.RemoveAt(0);
	}
}

void UBTService_SelectAttackType::NormalizeWeights(float& InOutMelee, float& InOutRanged) const
{
	constexpr float MinW = 5.f;
	InOutMelee  = FMath::Max(InOutMelee,  MinW);
	InOutRanged = FMath::Max(InOutRanged, MinW);
}