#include "BTService_AMTargetEvaluation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "FriendlyAI/Controller/AnimalMonsterAIController.h"
#include "FriendlyAI/AnimalMonster.h"

UBTService_AMTargetEvaluation::UBTService_AMTargetEvaluation()
{
	NodeName = TEXT("AM Target Evaluation");
	Interval = 0.3f;
	RandomDeviation = 0.05f;
}

void UBTService_AMTargetEvaluation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAnimalMonsterAIController* AIController = Cast<AAnimalMonsterAIController>(OwnerComp.GetAIOwner());
	if (!IsValid(AIController)) return;

	AAnimalMonster* Monster = Cast<AAnimalMonster>(AIController->GetPawn());
	if (!IsValid(Monster)) return;

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return;
	
	AIController->EvaluateTargetPriority();
	
	BlackboardComp->SetValueAsObject(TEXT("TargetActor"), Monster->CurrentTarget);
	BlackboardComp->SetValueAsObject(TEXT("PrimaryTarget"), Monster->PrimaryTarget);
	BlackboardComp->SetValueAsObject(TEXT("SecondaryTarget"), Monster->SecondaryTarget);
	BlackboardComp->SetValueAsObject(TEXT("BuildingTarget"), Monster->BuildingTarget);
}