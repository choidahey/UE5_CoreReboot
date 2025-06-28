#include "BTService_UpdateAggro.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/BossMonster/Season/SeasonBossMonster.h"
#include "MonsterAI/Components/MonsterAggroComponent.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"

UBTService_UpdateAggro::UBTService_UpdateAggro()
{
	NodeName = TEXT("Update Aggro Target");
	bNotifyTick = true;
	AggroTargetKey.SelectedKeyName = FAIKeys::TargetActor;
}

void UBTService_UpdateAggro::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!IsValid(AIC)) return;

	UBlackboardComponent* BB = AIC->GetBlackboardComponent();
	if (!IsValid(BB)) return;

	if (ASeasonBossMonster* Boss = Cast<ASeasonBossMonster>(AIC->GetPawn()))
	{
		if (Boss->AggroComp)
		{
			AActor* CurrentTarget = Cast<AActor>(BB->GetValueAsObject(AggroTargetKey.SelectedKeyName));
			AActor* AggroTarget = Boss->AggroComp->GetCurrentAggroTarget();

			if (!Boss->AggroComp->HasAnyAggro())
			{
				BB->SetValueAsObject(AggroTargetKey.SelectedKeyName,
					UGameplayStatics::GetPlayerPawn(Boss->GetWorld(), 0));
				return;
			}
			
			if (IsValid(AggroTarget))
			{
				if (AggroTarget != CurrentTarget)
					BB->SetValueAsObject(AggroTargetKey.SelectedKeyName, AggroTarget);
				return;
			}
			
			if (IsValid(CurrentTarget) && !Boss->AggroComp->HasAggroFor(CurrentTarget))
			{
				BB->ClearValue(AggroTargetKey.SelectedKeyName);
			}

		}
	}
}
