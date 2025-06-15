#include "BTTask_HelperRepair.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Gimmick/Components/DestructibleComponent.h"
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
    UE_LOG(LogTemp, Warning, TEXT("[Repair] BB is %s"), BB ? TEXT("Valid") : TEXT("Null"));
    if (!BB) return EBTNodeResult::Failed;

    AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject("ResourceTarget"));
    UE_LOG(LogTemp, Warning, TEXT("[Repair] TargetActor is %s"), TargetActor ? *TargetActor->GetName() : TEXT("Null"));
    if (!TargetActor) return EBTNodeResult::Failed;

    UDestructibleComponent* DestructibleComp = TargetActor->FindComponentByClass<UDestructibleComponent>();
    UE_LOG(LogTemp, Warning, TEXT("[Repair] DestructibleComp is %s"), DestructibleComp ? TEXT("Valid") : TEXT("Null"));
    if (!DestructibleComp) return EBTNodeResult::Failed;

    CachedTarget = TargetActor;
    CachedHelper = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
    ABaseHelperBot* Helper = CachedHelper ? Cast<ABaseHelperBot>(CachedHelper) : nullptr;
    if (Helper)
    {
        CachedRepairPerSecond = Helper->GetRepairingPerSecond();
        Helper->bIsChopping = true;
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

    UDestructibleComponent* DestructibleComp = CachedTarget->FindComponentByClass<UDestructibleComponent>();
    if (!DestructibleComp)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    const float RepairAmount = CachedRepairPerSecond * DeltaSeconds;
    DestructibleComp->Repair(RepairAmount);

    if (DestructibleComp->GetCurrentHealth() >= DestructibleComp->GetMaxHealth())
    {
        Helper->bIsChopping = false;
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}
