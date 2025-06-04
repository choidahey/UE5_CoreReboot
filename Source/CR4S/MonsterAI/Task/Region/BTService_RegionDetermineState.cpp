#include "MonsterAI/Task/Region/BTService_RegionDetermineState.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/Region/RegionBossMonster.h"
#include "MonsterAI/Components/MonsterAttributeComponent.h"
#include "MonsterAI/Components/MonsterStateComponent.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MonsterAI/MonsterAIHelper.h"

UBTService_RegionDetermineState::UBTService_RegionDetermineState()
{
	NodeName = TEXT("RegionDetermineState");
	Interval = 0.5f;

	bCallTickOnSearchStart = true;
}

void UBTService_RegionDetermineState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	ABaseMonster* Monster = MonsterAIHelper::GetControlledMonster(OwnerComp);
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!Monster || !BlackboardComp) return;

	const bool bIsPlayingAttackMontage = BlackboardComp->GetValueAsBool(FAIKeys::bIsPlayingAttackMontage);
	if (bIsPlayingAttackMontage) return;

	AActor* Target = BlackboardComp ? Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName)) : nullptr;
	if (!IsValid(Target)) return;

	UMonsterStateComponent* StateComp = Monster->FindComponentByClass<UMonsterStateComponent>();
	UMonsterAttributeComponent* AttrComp = Monster->FindComponentByClass<UMonsterAttributeComponent>();
	if (!StateComp || !AttrComp) return;

	const float DistanceSquare = FVector::DistSquared(Target->GetActorLocation(), Monster->GetActorLocation());
	const FMonsterAttributeRow& Attribute = Monster->FindComponentByClass<UMonsterAttributeComponent>()->GetMonsterAttribute();

	const float SightRangeSquare = FMath::Square(Attribute.SightRadius);
	const float CombatRangeSquare = FMath::Square(Attribute.CombatRadius);

	if (StateComp->IsInState(EMonsterState::Patrol))
	{
		if (DistanceSquare <= SightRangeSquare)
		{
			StateComp->SetState(EMonsterState::Alert);
		}
	}
	else if (StateComp->IsInState(EMonsterState::Alert))
	{
		if (DistanceSquare > SightRangeSquare)
		{
			StateComp->SetState(EMonsterState::Patrol);
		}
		else if (DistanceSquare <= CombatRangeSquare)
		{
			StateComp->SetState(EMonsterState::Combat);
		}
	}
	else if (StateComp->IsInState(EMonsterState::Combat))
	{
		if (const ARegionBossMonster* RegionBoss = Cast<ARegionBossMonster>(Monster))
		{
			if (RegionBoss->IsOutsideCombatRange())
			{
				StateComp->SetState(EMonsterState::Return);
				BlackboardComp->SetValueAsBool(FAIKeys::CanSeePlayer, false);
			}
		}
	}
}
