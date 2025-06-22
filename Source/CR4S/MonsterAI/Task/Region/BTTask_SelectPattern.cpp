#include "MonsterAI/Task/Region/BTTask_SelectPattern.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "MonsterAI/Region/RegionBossMonster.h"
#include "MonsterAI/Data/RegionBossPatternDataAsset.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"

UBTTask_SelectPattern::UBTTask_SelectPattern()
{
	NodeName = TEXT("Select Pattern");
}

EBTNodeResult::Type UBTTask_SelectPattern::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ARegionBossMonster* Monster = Cast<ARegionBossMonster>(OwnerComp.GetAIOwner()->GetPawn());
	if (!IsValid(Monster) || !Monster->PatternDataAsset) return EBTNodeResult::Failed;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	const AActor* Target = Cast<AActor>(BB->GetValueAsObject(FAIKeys::TargetActor));
	if (!Target) return EBTNodeResult::Failed;

	const float Distance = FVector::Dist(Monster->GetActorLocation(), Target->GetActorLocation());

	const uint8 PhaseValue = BB->GetValueAsInt(FRegionBossAIKeys::CurrentPhase);
	const EBossPhase Phase = static_cast<EBossPhase>(PhaseValue);
	const uint8 LastPatternID = BB->GetValueAsInt(FRegionBossAIKeys::PreviousPatternID);

	const TArray<FRegionPatternData>& AllPatterns = Monster->PatternDataAsset->PatternList;

	// Step 1: Filter valid candidate patterns by distance and phase
	TArray<const FRegionPatternData*> Candidates;
	TArray<float> Weights;
	float TotalWeight = 0.f;

	for (const FRegionPatternData& Pattern : AllPatterns)
	{
		// Skip previously used pattern
		if (Pattern.PatternID == LastPatternID || Pattern.PatternID == 0)
			continue;

		if (Distance < Pattern.DistanceRange.X || Distance > Pattern.DistanceRange.Y)
			continue;

		const bool bValid = Pattern.PhaseSequences.ContainsByPredicate(
			[&](const FPhaseSkillSequence& Seq)
			{
				return Seq.Phase == Phase && !Seq.SkillIndices.IsEmpty();
			});

		if (!bValid)
			continue;

		Candidates.Add(&Pattern);
		Weights.Add(Pattern.PatternWeight);
		TotalWeight += Pattern.PatternWeight;
	}

	if (Candidates.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] No matching pattern found. Distance: %.1f Phase: %d"), *MyHeader, Distance, PhaseValue);
		return EBTNodeResult::Failed;
	}

	// Step 2: Weighted random selection
	const float Roll = FMath::FRandRange(0.f, TotalWeight);
	float Accumulated = 0.f;

	for (int32 i = 0; i < Candidates.Num(); ++i)
	{
		Accumulated += Weights[i];
		if (Roll <= Accumulated)
		{
			const uint8 SelectedID = Candidates[i]->PatternID;

			BB->SetValueAsInt(FRegionBossAIKeys::CurrentPatternID, SelectedID);
			BB->SetValueAsInt(FRegionBossAIKeys::PatternStepIndex, 0);

			UE_LOG(LogTemp, Log, TEXT("[%s] Selected PatternID: %d (Prev: %d, Phase: %d, Distance: %.1f)"),
				*MyHeader, SelectedID, LastPatternID, PhaseValue, Distance);

			return EBTNodeResult::Succeeded;
		}
	}

	// Fallback : Should not reach here
	UE_LOG(LogTemp, Warning, TEXT("[%s] Pattern selection failed unexpectedly."), *MyHeader);
	return EBTNodeResult::Failed;
}
