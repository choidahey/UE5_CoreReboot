#include "BTTask_HelperChopWood.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "../../Gimmick/Components/DestructibleComponent.h"
#include "../../Gimmick/GimmickObjects/ResourceGimmick/TreeGimmick.h"
#include "../BaseHelperBot.h"
#include "../Controller/HelperBotAIController.h"

UBTTask_HelperChopWood::UBTTask_HelperChopWood()
{
	NodeName = TEXT("HelperChopWood");
	bNotifyTick = true;	
}

EBTNodeResult::Type UBTTask_HelperChopWood::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(ResourceTargetKey.SelectedKeyName));
	if (!TargetActor) return EBTNodeResult::Failed;

	CachedTarget = TargetActor;
	CachedHelper = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;

	ABaseHelperBot* Helper = Cast<ABaseHelperBot>(CachedHelper);
	if (Helper)
	{
		CachedDamagePerSecond = Helper->GetWoodDamagePerSecond();
	}
	return (CachedHelper && CachedTarget) ? EBTNodeResult::InProgress : EBTNodeResult::Failed;
}

void UBTTask_HelperChopWood::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!CachedTarget || !CachedHelper)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	ABaseHelperBot* Helper = Cast<ABaseHelperBot>(CachedHelper);
	if (!Helper)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	const float DamageThisFrame = CachedDamagePerSecond * DeltaSeconds;
		
	if (!IsValid(CachedTarget) || CachedTarget->IsActorBeingDestroyed())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	AHelperBotAIController* HelperBotController = Cast<AHelperBotAIController>(OwnerComp.GetAIOwner());
	ATreeGimmick* Tree = Cast<ATreeGimmick>(CachedTarget);
	if (IsValid(Tree) && IsValid(Tree->GetDestructibleComponent()))
	{
		Tree->GetDestructibleComponent()->TakeDamage(HelperBotController, DamageThisFrame);
	}
}
