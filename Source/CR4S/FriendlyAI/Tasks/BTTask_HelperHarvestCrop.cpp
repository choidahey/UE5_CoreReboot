#include "BTTask_HelperHarvestCrop.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "AIController.h"
#include "Gimmick/GimmickObjects/Farming/CropsGimmick.h"
#include "../BaseHelperBot.h"
#include "../Controller/HelperBotAIController.h"

UBTTask_HelperHarvestCrop::UBTTask_HelperHarvestCrop()
{
	NodeName = TEXT("HelperHarvestCrop");
	bNotifyTick = true;	
}

EBTNodeResult::Type UBTTask_HelperHarvestCrop::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(ResourceTargetKey.SelectedKeyName));
	if (!TargetActor) 
	{
		return EBTNodeResult::Failed;
	}

	ACropsGimmick* Crop = Cast<ACropsGimmick>(TargetActor);
	if (!Crop)
	{
		return EBTNodeResult::Failed;
	}

	if (!Crop->GetIsHarvestable())
	{
		return EBTNodeResult::Failed;
	}

	CachedTarget = TargetActor;
	CachedHelper = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;

	ABaseHelperBot* Helper = Cast<ABaseHelperBot>(CachedHelper);
	if (Helper)
	{
		CachedHarvestDuration = Helper->GetHarvestDuration();
		Helper->SetIsWorking(true);
		ElapsedTime = 0.f;
	}

	if (Helper && TargetActor)
	{
		Helper->UpdateEyeBeamWorkTarget(TargetActor);
	}
	
	return (CachedHelper && CachedTarget) ? EBTNodeResult::InProgress : EBTNodeResult::Failed;
}

void UBTTask_HelperHarvestCrop::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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
	
	ACropsGimmick* Crop = Cast<ACropsGimmick>(CachedTarget);
	if (!Crop || !Crop->GetIsHarvestable())
	{
		if (Helper)
		{
			Helper->SetIsWorking(false);
			Helper->StopEyeBeamWork();
		}
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (!IsValid(CachedTarget) || CachedTarget->IsActorBeingDestroyed())
	{
		if (Helper)
		{
			Helper->SetIsWorking(false);
			Helper->StopEyeBeamWork();
		}
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	ElapsedTime += DeltaSeconds;

	if (ElapsedTime >= CachedHarvestDuration)
	{
		Crop->Harvest(Helper);
		
		if (Helper)
		{
			Helper->SetIsWorking(false);
			Helper->StopEyeBeamWork();
		}
		
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}