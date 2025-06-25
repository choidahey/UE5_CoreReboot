#include "BTTask_HelperRepair.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Gimmick/GimmickObjects/Buildings/BaseBuildingGimmick.h"
#include "FriendlyAI/BaseHelperBot.h"
#include "FriendlyAI/Controller/HelperBotAIController.h"

UBTTask_HelperRepair::UBTTask_HelperRepair()
{
    NodeName = TEXT("HelperRepair");
    bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_HelperRepair::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

    if (!TargetActor->IsA(ABaseBuildingGimmick::StaticClass()))
    {
        return EBTNodeResult::Failed;
    }

    CachedTarget = TargetActor;
    CachedTarget->OnDestroyed.RemoveDynamic(this, &UBTTask_HelperRepair::OnTargetDestroyed);
    CachedTarget->OnDestroyed.AddDynamic(this, &UBTTask_HelperRepair::OnTargetDestroyed);
    CachedHelper = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;

    ABaseHelperBot* Helper = Cast<ABaseHelperBot>(CachedHelper);
    if (Helper)
    {
        CachedRepairPerSecond = Helper->GetRepairingPerSecond();
        Helper->SetIsWorking(true);
    }

    if (Helper && TargetActor)
    {
        Helper->UpdateEyeBeamWorkTarget(TargetActor);
    }

    return (CachedHelper && CachedTarget) ? EBTNodeResult::InProgress : EBTNodeResult::Failed;
}

void UBTTask_HelperRepair::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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

    if (!IsValid(CachedTarget) || CachedTarget->IsActorBeingDestroyed())
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }

    ABaseBuildingGimmick* BuildingGimmick = Cast<ABaseBuildingGimmick>(CachedTarget);
    if (!BuildingGimmick)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    if (BuildingGimmick->GetDurabilityRatio() >= 1.0f)
    {
        if (Helper)
        {
            Helper->SetIsWorking(false);
            Helper->StopEyeBeamWork();
        }

        if (CachedTarget)
        {
            CachedTarget->OnDestroyed.RemoveDynamic(this, &UBTTask_HelperRepair::OnTargetDestroyed);
        }

        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }

    const float RepairAmount = CachedRepairPerSecond * DeltaSeconds;
    BuildingGimmick->OnRepairBuilding(RepairAmount);
}

void UBTTask_HelperRepair::OnTargetDestroyed(AActor*)
{
    if (ABaseHelperBot* Helper = Cast<ABaseHelperBot>(CachedHelper))
    {
        Helper->SetIsWorking(false);
        Helper->StopEyeBeamWork();
    }
    if (CachedTarget)
    {
        CachedTarget->OnDestroyed.RemoveDynamic(this, &UBTTask_HelperRepair::OnTargetDestroyed);
        CachedTarget = nullptr;
    }
    if (UBehaviorTreeComponent* BTC = Cast<UBehaviorTreeComponent>(GetOuter()))
    {
        FinishLatentTask(*BTC, EBTNodeResult::Succeeded);
    }
}