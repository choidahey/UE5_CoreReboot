#include "BaseMonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "CR4S/MonsterAI/BaseMonster.h"

ABaseMonsterAIController::ABaseMonsterAIController()
{
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComp"));
}

void ABaseMonsterAIController::BeginPlay()
{
	Super::BeginPlay();

}

void ABaseMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (const ABaseMonster* Monster = Cast<ABaseMonster>(InPawn))
	{
		if (UBehaviorTree* BT = Monster->BehaviorTree)
		{
			UseBlackboard(BT->BlackboardAsset, BlackboardComp);
			RunBehaviorTree(BT);
		}
	}
}
