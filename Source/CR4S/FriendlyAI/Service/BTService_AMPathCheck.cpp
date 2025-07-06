#include "BTService_AMPathCheck.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "FriendlyAI/Controller/AnimalMonsterAIController.h"
#include "FriendlyAI/AnimalMonster.h"

UBTService_AMPathCheck::UBTService_AMPathCheck()
{
	NodeName = TEXT("AM Path Check");
	Interval = 0.5f;
	RandomDeviation = 0.1f;
}

void UBTService_AMPathCheck::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAnimalMonsterAIController* AIController = Cast<AAnimalMonsterAIController>(OwnerComp.GetAIOwner());
	if (!IsValid(AIController)) return;

	AAnimalMonster* Monster = Cast<AAnimalMonster>(AIController->GetPawn());
	if (!IsValid(Monster)) return;

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	AActor* PlayerTarget = Cast<AActor>(BlackboardComp->GetValueAsObject(TEXT("PrimaryTarget")));
	bool bCanReach = AIController->CheckPathToTarget(PlayerTarget);
    
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsBool(TEXT("bCanReachPlayer"), bCanReach);
	}
}