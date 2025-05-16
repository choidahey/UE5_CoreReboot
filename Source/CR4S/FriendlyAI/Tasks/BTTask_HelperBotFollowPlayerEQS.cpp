#include "BTTask_HelperBotFollowPlayerEQS.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "Navigation/PathFollowingComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_HelperBotFollowPlayerEQS::UBTTask_HelperBotFollowPlayerEQS(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    NodeName = TEXT("HelperBot Follow Player via EQS");
    EQSRequest.RunMode = EEnvQueryRunMode::SingleResult;
}

EBTNodeResult::Type UBTTask_HelperBotFollowPlayerEQS::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    CachedOwnerComp  = &OwnerComp;
    CachedController = OwnerComp.GetAIOwner();

    if (!CachedController || !EQSRequest.QueryTemplate)
    {
        return EBTNodeResult::Failed;
    }
    
    return Super::ExecuteTask(OwnerComp, NodeMemory);
}

void UBTTask_HelperBotFollowPlayerEQS::OnQueryFinished(
    UEnvQueryInstanceBlueprintWrapper* QueryInstance,
    EEnvQueryStatus::Type QueryStatus)
{
    if (!CachedOwnerComp || !CachedController || QueryStatus != EEnvQueryStatus::Success)
    {
        FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Failed);
        return;
    }
    
    TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
    if (Locations.Num() == 0)
    {
        FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Failed);
        return;
    }

    FAIMoveRequest MoveReq;
    MoveReq.SetGoalLocation(Locations[0]);
    MoveReq.SetAcceptanceRadius(AcceptableRadius);

    FPathFollowingRequestResult MoveResult = CachedController->MoveTo(MoveReq);

    FinishLatentTask(
        *CachedOwnerComp,
        (MoveResult.Code == EPathFollowingRequestResult::RequestSuccessful)
            ? EBTNodeResult::Succeeded
            : EBTNodeResult::Failed
    );
}
