#include "BTTask_HelperChopWood.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "DestructibleInterface.h"

UBTTask_HelperChopWood::UBTTask_HelperChopWood()
{
	NodeName = TEXT("HelperChopWood");
}

EBTNodeResult::Type UBTTask_HelperChopWood::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory
)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	AActor* TargetActor = Cast<AActor>(
	BlackboardComp->GetValueAsObject(ResourceTargetKey.SelectedKeyName)
);
	if (!TargetActor)
	{
		return EBTNodeResult::Failed;
	}
		
	// const float DamageAmount = 10.f;
	// APawn* HelperBotPawn = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn());
	// AController* HelperBotController = OwnerComp.GetAIOwner();
	//
	// UGameplayStatics::ApplyDamage(TargetActor, DamageAmount, HelperBotController, HelperBotPawn, UDamageType::StaticClass());

	TargetActor->Destroy();
	
	return EBTNodeResult::Succeeded;
}
