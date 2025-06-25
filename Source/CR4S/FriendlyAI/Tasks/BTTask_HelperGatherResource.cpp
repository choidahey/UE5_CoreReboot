#include "BTTask_HelperGatherResource.h" 
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "../../Gimmick/Components/DestructibleComponent.h"
#include "../../Gimmick/GimmickObjects/DestructibleGimmick.h"
#include "../BaseHelperBot.h"
#include "../../Inventory/Components/PlayerInventoryComponent.h"
#include "Gimmick/GimmickObjects/ResourceGimmick/TreeGimmick.h"
#include "../Controller/HelperBotAIController.h"
#include "Engine/Engine.h"

UBTTask_HelperGatherResource::UBTTask_HelperGatherResource()
{
	NodeName = TEXT("HelperChopWood2");
	bNotifyTick = true;	
}

EBTNodeResult::Type UBTTask_HelperGatherResource::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		return EBTNodeResult::Failed;
	}

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject("TargetActor"));
	if (!TargetActor)
	{
		return EBTNodeResult::Failed;
	}

	if (!TargetActor->IsA(ADestructibleGimmick::StaticClass()))
	{
		return EBTNodeResult::Failed;
	}
	
	CachedTarget = TargetActor;
	CachedTarget->OnDestroyed.RemoveDynamic(this, &UBTTask_HelperGatherResource::OnTargetDestroyed);
	CachedTarget->OnDestroyed.AddDynamic(this, &UBTTask_HelperGatherResource::OnTargetDestroyed);
	CachedHelper = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;

	ABaseHelperBot* Helper = Cast<ABaseHelperBot>(CachedHelper);
	if (Helper)
	{
		CachedDamagePerSecond = Helper->GetWoodDamagePerSecond();
		Helper->SetIsWorking(true);
	}

	if (Helper && TargetActor)
	{
		Helper->UpdateEyeBeamWorkTarget(TargetActor);
	}
	
	return (CachedHelper && CachedTarget) ? EBTNodeResult::InProgress : EBTNodeResult::Failed;
}

void UBTTask_HelperGatherResource::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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
	
	// ABaseHelperBot* Helper = Cast<ABaseHelperBot>(CachedHelper);
	// if (Helper && TargetActor)
	// {
	// 	Helper->UpdateEyeBeamWorkTarget(TargetActor);
	// }

	UGameplayStatics::ApplyDamage(CachedTarget, DamageThisFrame, Helper->GetController(), Helper, UDamageType::StaticClass());
}

void UBTTask_HelperGatherResource::OnTargetDestroyed(AActor* /*DestroyedActor*/)
{
	if (ABaseHelperBot* Helper = Cast<ABaseHelperBot>(CachedHelper))
	{
		Helper->SetIsWorking(false);
		Helper->StopEyeBeamWork();
	}
	if (CachedTarget)
	{
		CachedTarget->OnDestroyed.RemoveDynamic(this, &UBTTask_HelperGatherResource::OnTargetDestroyed);
		CachedTarget = nullptr;
	}
	if (UBehaviorTreeComponent* BTC = Cast<UBehaviorTreeComponent>(GetOuter()))
	{
		FinishLatentTask(*BTC, EBTNodeResult::Succeeded);
	}
}
