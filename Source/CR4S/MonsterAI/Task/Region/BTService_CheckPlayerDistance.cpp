#include "MonsterAI/Task/Region/BTService_CheckPlayerDistance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/Components/MonsterAttributeComponent.h"
#include "MonsterAI/MonsterAIHelper.h"

UBTService_CheckPlayerDistance::UBTService_CheckPlayerDistance()
{
	NodeName = TEXT("CheckPlayerDistance");
	Interval = 0.5f;
}

void UBTService_CheckPlayerDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		return;
	}

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
	ABaseMonster* Monster = MonsterAIHelper::GetControlledMonster(OwnerComp);
	if (!Target || !Monster)
	{
		return;
	}

	const float Distance = FVector::DistSquared(Target->GetActorLocation(), Monster->GetActorLocation());
	const FMonsterAttributeRow& Attribute = Monster->GetAttributeComponent()->GetMonsterAttribute();
	
	if (Distance <= Attribute.CombatRadius * Attribute.CombatRadius)
	{
		BB->SetValueAsInt(NextStateKey.SelectedKeyName, (int32)EMonsterState::Combat);
	}
	else if (Distance <= Attribute.SightRadius * Attribute.SightRadius)
	{
		BB->SetValueAsInt(NextStateKey.SelectedKeyName, (int32)EMonsterState::Alert);
	}
}
