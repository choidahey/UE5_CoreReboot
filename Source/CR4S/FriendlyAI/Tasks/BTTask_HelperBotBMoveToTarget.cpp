#include "BTTask_HelperBotBMoveToTarget.h"
#include "../Component/AIJumpComponent.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "../Component/AIJumpComponent.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

UBTTask_HelperBotBMoveToTarget::UBTTask_HelperBotBMoveToTarget()
{
	NodeName    = TEXT("HelperBot tt");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_HelperBotBMoveToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn())
    {
        if (UAIJumpComponent* JumpComp = Pawn->FindComponentByClass<UAIJumpComponent>())
        {
            JumpComp->ActivateJumpComponent();
        }
    }
    return EBTNodeResult::InProgress;
}

void UBTTask_HelperBotBMoveToTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AAIController* AICon = OwnerComp.GetAIOwner();
    APawn* Pawn = AICon ? AICon->GetPawn() : nullptr;
    AActor* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

    if (!Pawn || !PlayerPawn)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    const float Distance = FVector::Dist(Pawn->GetActorLocation(), PlayerPawn->GetActorLocation());
    
    if (UAIJumpComponent* JumpComp = Pawn->FindComponentByClass<UAIJumpComponent>())
    {
        if (Distance >= DistanceThreshold)
        {
            if (!JumpComp->IsActive())
                JumpComp->Activate(true);
        }
        else
        {
            if (JumpComp->IsActive())
                JumpComp->Deactivate();
        }
    }
}