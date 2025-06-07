#include "MonsterAI/Task/Region/BTTask_RunSkillEQS.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "MonsterAI/Data/MonsterEnum.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "MonsterAI/Region/RegionBossMonster.h"
#include "MonsterAI/MonsterAIHelper.h"

UBTTask_RunSkillEQS::UBTTask_RunSkillEQS()
{
	NodeName = TEXT("Run Skill EQS");

	SkillIndexKey.SelectedKeyName = FRegionBossAIKeys::SkillIndex;
	SkillTargetLocationKey.SelectedKeyName = FRegionBossAIKeys::SkillTargetLocation;
	ApproachTypeKey.SelectedKeyName = FRegionBossAIKeys::ApproachType;
}

EBTNodeResult::Type UBTTask_RunSkillEQS::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	CachedOwnerComp = &OwnerComp;

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!IsValid(BlackboardComp)) return EBTNodeResult::Failed;

	ABaseMonster* Monster = MonsterAIHelper::GetControlledMonster(OwnerComp);
	ARegionBossMonster* RegionBoss = Cast<ARegionBossMonster>(Monster);
	if (!RegionBoss) return EBTNodeResult::Failed;

	CurrentSkillIndex = BlackboardComp->GetValueAsInt(SkillIndexKey.SelectedKeyName);

	const TArray<EApproachType>& Candidates = RegionBoss->GetApproachCandidates(CurrentSkillIndex);
	if (Candidates.IsEmpty()) return EBTNodeResult::Failed;

	CurrentApproachType = DecideApproachType(Candidates, RegionBoss->GetActorLocation(), SkillTargetLocation);

	UEnvQuery* EQS = RegionBoss->GetEQSByApproachType(CurrentApproachType);
	if (!EQS) return EBTNodeResult::Failed;

	RunEQS(EQS, RegionBoss);
	return EBTNodeResult::InProgress;
}

void UBTTask_RunSkillEQS::RunEQS(UEnvQuery* EQS, UObject* QueryOwner)
{
	FEnvQueryRequest Request(EQS, QueryOwner);
	Request.Execute(EEnvQueryRunMode::SingleResult, this, &UBTTask_RunSkillEQS::OnEQSFinished);
}

void UBTTask_RunSkillEQS::OnEQSFinished(TSharedPtr<FEnvQueryResult> Result)
{
	if (!Result.IsValid() || !CachedOwnerComp.IsValid())
	{
		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (!Result->IsSuccessful() || Result->Items.Num() == 0)
	{
		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Failed);
		return;
	}

	SkillTargetLocation = Result->GetItemAsLocation(0);

	UBlackboardComponent* BlackboardComp = CachedOwnerComp->GetBlackboardComponent();
	if (!BlackboardComp)
	{
		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Failed);
		return;
	}

	BlackboardComp->SetValueAsVector(SkillTargetLocationKey.SelectedKeyName, SkillTargetLocation);
	BlackboardComp->SetValueAsEnum(ApproachTypeKey.SelectedKeyName, static_cast<uint8>(CurrentApproachType));

	UE_LOG(LogTemp, Log, TEXT("[RunSkillEQS] TargetLocation: %s | ApproachType: %s"),
		*SkillTargetLocation.ToString(),
		*UEnum::GetValueAsString(CurrentApproachType));

	FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
	
}

EApproachType UBTTask_RunSkillEQS::DecideApproachType(const TArray<EApproachType>& Candidates, const FVector& From, const FVector& To) const
{
	const float Distance = FVector::Dist(From, To);

	if (Candidates.Contains(EApproachType::Stalk) && Candidates.Contains(EApproachType::Dash))
		return (Distance < 1000.f) ? EApproachType::Stalk : EApproachType::Dash;

	if (Candidates.Contains(EApproachType::Stand) && Candidates.Contains(EApproachType::Flank))
		return (Distance < 600.f) ? EApproachType::Stand : EApproachType::Flank;

	if (Candidates.Contains(EApproachType::AdvanceCast))
		return EApproachType::AdvanceCast;

	if (Candidates.Num() > 0)
		return Candidates[0];

	return EApproachType::Stand;; // fallback
}
