#include "BTService_UpdateAggro.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "MonsterAI/BossMonster/Season/SeasonBossMonster.h"
#include "MonsterAI/Components/MonsterAggroComponent.h"

UBTService_UpdateAggro::UBTService_UpdateAggro()
{
	NodeName    = TEXT("Update Aggro Target");
	bNotifyTick = true;
}

void UBTService_UpdateAggro::TickNode(UBehaviorTreeComponent& OwnerComp,
									  uint8* NodeMemory,
									  float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (AAIController* AIC = OwnerComp.GetAIOwner())
	{
		if (ASeasonBossMonster* Boss = Cast<ASeasonBossMonster>(AIC->GetPawn()))
		{
			if (Boss->AggroComp)
			{
				Boss->AggroComp->ApplyToBlackboard(OwnerComp.GetBlackboardComponent(), Boss->AggroTargetKey);
			}
		}
	}
}
