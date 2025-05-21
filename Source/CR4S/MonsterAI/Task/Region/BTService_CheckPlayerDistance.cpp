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
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	ABaseMonster* Monster = MonsterAIHelper::GetControlledMonster(OwnerComp);

	AActor* Target = BB ? Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName)) : nullptr;
	if (!Monster || !Target)
	{
		return;
	}

	const float Distance = FVector::DistSquared(Target->GetActorLocation(), Monster->GetActorLocation());
	const FMonsterAttributeRow& Attribute = Monster->GetAttributeComponent()->GetMonsterAttribute();

	if (UMonsterStateComponent* StateComp = Monster->GetStateComponent())
	{
		const EMonsterState CurrentState = StateComp->GetCurrentState();
		if (CurrentState == EMonsterState::Idle && Distance <= FMath::Square(Attribute.SightRadius))
		{
			StateComp->SetState(EMonsterState::Alert);
		}
		else if (CurrentState == EMonsterState::Alert && Distance > FMath::Square(Attribute.SightRadius))
		{
			StateComp->SetState(EMonsterState::Idle);
		}
		else if(CurrentState == EMonsterState::Alert && Distance <= FMath::Square(Attribute.CombatRadius))
		{
			StateComp->SetState(EMonsterState::Combat);
		}
	}
}
