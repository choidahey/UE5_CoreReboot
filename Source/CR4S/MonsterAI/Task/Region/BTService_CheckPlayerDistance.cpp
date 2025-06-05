#include "MonsterAI/Task/Region/BTService_CheckPlayerDistance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/Components/MonsterAttributeComponent.h"
#include "MonsterAI/Components/MonsterStateComponent.h"
#include "MonsterAI/MonsterAIHelper.h"

UBTService_CheckPlayerDistance::UBTService_CheckPlayerDistance()
{
	NodeName = TEXT("CheckPlayerDistance");
	Interval = 0.5f;
}

void UBTService_CheckPlayerDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ABaseMonster* Monster = MonsterAIHelper::GetControlledMonster(OwnerComp);
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	AActor* Target = BlackboardComp ? Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName)) : nullptr;
	if (!Monster || !Target) return;

	const float Distance = FVector::DistSquared(Target->GetActorLocation(), Monster->GetActorLocation());
	const FMonsterAttributeRow& Attribute = Monster->FindComponentByClass<UMonsterAttributeComponent>()->GetMonsterAttribute();

	if (UMonsterStateComponent* StateComp = Monster->FindComponentByClass<UMonsterStateComponent>())
	{
		if (StateComp->IsInState(EMonsterState::Patrol) && Distance <= FMath::Square(Attribute.SightRadius))
		{
			StateComp->SetState(EMonsterState::Alert);
		}
		else if (StateComp->IsInState(EMonsterState::Alert) && Distance > FMath::Square(Attribute.SightRadius))
		{
			StateComp->SetState(EMonsterState::Patrol);
		}
		else if(StateComp->IsInState(EMonsterState::Alert) && Distance <= FMath::Square(Attribute.CombatRadius))
		{
			StateComp->SetState(EMonsterState::Combat);
		}
	}
}
