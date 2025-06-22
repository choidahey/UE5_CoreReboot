#include "MonsterAI/Task/Region/BTTask_IncrementPatternStep.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "MonsterAI/Region/RegionBossMonster.h"
#include "MonsterAI/Data/RegionBossPatternDataAsset.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"

UBTTask_IncrementPatternStep::UBTTask_IncrementPatternStep()
{
	NodeName = TEXT("Increment Pattern Step");
}

EBTNodeResult::Type UBTTask_IncrementPatternStep::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ARegionBossMonster* Monster = Cast<ARegionBossMonster>(OwnerComp.GetAIOwner()->GetPawn());
	if (!IsValid(Monster) || !Monster->PatternDataAsset) return EBTNodeResult::Failed;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	const uint8 PatternID = BB->GetValueAsInt(FRegionBossAIKeys::CurrentPatternID);
	if (PatternID == -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] CurrentPatternID is None."), *MyHeader);
		return EBTNodeResult::Failed;
	}

	const uint8 PhaseValue = BB->GetValueAsInt(FRegionBossAIKeys::CurrentPhase);
	const EBossPhase Phase = static_cast<EBossPhase>(PhaseValue);

	const FRegionPatternData* PatternData = Monster->PatternDataAsset->PatternList.FindByPredicate(
		[&](const FRegionPatternData& P) { return P.PatternID == PatternID; });

	if (!PatternData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] Pattern ID %d not found in data asset."), *MyHeader, PatternID);
		return EBTNodeResult::Failed;
	}

	const FPhaseSkillSequence* Sequence = PatternData->PhaseSequences.FindByPredicate(
		[&](const FPhaseSkillSequence& Seq) { return Seq.Phase == Phase; });

	if (!Sequence || Sequence->SkillIndices.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] No skill sequence found for phase %d."), *MyHeader, PhaseValue);
		return EBTNodeResult::Failed;
	}

	int32 StepIndex = BB->GetValueAsInt(FRegionBossAIKeys::PatternStepIndex);
	++StepIndex;

	if (StepIndex < Sequence->SkillIndices.Num())
	{
		// Advance to next step
		BB->SetValueAsInt(FRegionBossAIKeys::PatternStepIndex, StepIndex);

		UE_LOG(LogTemp, Log, TEXT("[%s] Pattern %d Step -> %d"), *MyHeader, PatternID, StepIndex);
	}
	else
	{
		// End of pattern ¡æ reset & update PreviousPatternID
		BB->SetValueAsInt(FRegionBossAIKeys::PreviousPatternID, PatternID);
		BB->SetValueAsInt(FRegionBossAIKeys::CurrentPatternID, -1);
		BB->SetValueAsInt(FRegionBossAIKeys::PatternStepIndex, 0);

		UE_LOG(LogTemp, Log, TEXT("[%s] Pattern %d finished. Reset pattern state."), *MyHeader, PatternID);
	}

	return EBTNodeResult::Succeeded;
}
 