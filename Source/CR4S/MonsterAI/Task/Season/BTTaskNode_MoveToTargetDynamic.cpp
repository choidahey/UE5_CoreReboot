#include "MonsterAI/Task/Season/BTTaskNode_MoveToTargetDynamic.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"
#include "CR4S.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/Components/MonsterStateComponent.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "Navigation/PathFollowingComponent.h"

UBTTaskNode_MoveToTargetDynamic::UBTTaskNode_MoveToTargetDynamic()
{
	NodeName = TEXT("MoveToTargetDynamic");
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTaskNode_MoveToTargetDynamic::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
    if (!AIC)
    {
        UE_LOG(LogTemp, Warning, TEXT("[ChasePlayerDynamic] ExecuteTask: AIC is nullptr → Failed"));
        return EBTNodeResult::Failed;
    }
	
	EPathFollowingStatus::Type PFStatus = AIC->GetPathFollowingComponent()->GetStatus();
	UE_LOG(LogTemp, Warning, TEXT("[MoveToTargetDynamic::TickTask] PathFollowing Status = %d"), (int)PFStatus);
	
	APawn* OwnerPawn = AIC->GetPawn();
	if (!IsValid(OwnerPawn))
	{
		UE_LOG(LogTemp, Warning, TEXT("[MoveToTargetDynamic] ExecuteTask: Pawn is invalid → Failed"));
		return EBTNodeResult::Failed;
	}


    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB)
    {
        UE_LOG(LogTemp, Warning, TEXT("[ChasePlayerDynamic] ExecuteTask: BlackboardComp is nullptr → Failed"));
        return EBTNodeResult::Failed;
    }
	
	const FName KeyName = TargetActorKey.SelectedKeyName;
	// AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(KeyName));
	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(FSeasonBossAIKeys::NearestHouseActor));
	
    if (!IsValid(TargetActor))
    {
    	return EBTNodeResult::Failed;
   //  	AActor* PlayerPawn = UGameplayStatics::GetPlayerPawn(OwnerComp.GetWorld(), 0);
   //  	if (IsValid(PlayerPawn))
   //  	{
   //  		TargetActor = PlayerPawn;
   //  		UE_LOG(LogTemp, Warning,
			// 	TEXT("[MoveToTargetDynamic] Blackboard Key='%s' invalid → Fallback to PlayerPawn ptr=%p (Name='%s')"),
			// 	*KeyName.ToString(),
			// 	PlayerPawn,
			// 	*PlayerPawn->GetName()
			// );
   //  	}
   //  	else
   //  	{
   //  		UE_LOG(LogTemp, Warning,
			// 	TEXT("[MoveToTargetDynamic] Neither Blackboard Key='%s' nor PlayerPawn is valid → Failed"),
			// 	*KeyName.ToString()
			// );
   //  		return EBTNodeResult::Failed;
   //  	}
    }
    else
    {
	    CR4S_Log(LogDa, Log, TEXT("[MoveToTargetDynamic] Success Get Target"));
    }

	if (UMonsterStateComponent* StateComp = OwnerPawn->FindComponentByClass<UMonsterStateComponent>())
	{
		StateComp->SetState(MovementState);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[MoveToTargetDynamic] ExecuteTask: Pawn에 MonsterStateComponent Pawn=%s"),
			*OwnerPawn->GetName());
	}
	
	CR4S_Log(LogDa, Warning, TEXT("[ChasePlayerDynamic] Move To Target!!!!"));

	EPathFollowingRequestResult::Type PFResult = AIC->MoveToActor(
		TargetActor,
		AcceptanceRadius,
		false,
		true,
		true,
		nullptr,
		false
	);

	if (PFResult == EPathFollowingRequestResult::Failed)
	{
		FVector PawnLoc = OwnerPawn->GetActorLocation();
		FVector GoalLoc = TargetActor->GetActorLocation();
		UE_LOG(LogTemp, Warning,
			TEXT("[MoveToTargetDynamic] MoveToActor FAILED! PawnLoc=(%.1f,%.1f,%.1f) GoalLoc=(%.1f,%.1f,%.1f) Acc=%.1f"),
			PawnLoc.X, PawnLoc.Y, PawnLoc.Z,
			GoalLoc.X, GoalLoc.Y, GoalLoc.Z,
			AcceptanceRadius
		);
		return EBTNodeResult::Failed;
	}
	else if (PFResult == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[MoveToTargetDynamic] MoveToActor: Already at goal (AcceptanceRadius=%.1f)"),
			AcceptanceRadius
		);
		return EBTNodeResult::Succeeded;
	}
	{
		return EBTNodeResult::InProgress;
	}
}

void UBTTaskNode_MoveToTargetDynamic::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	APawn* SelfPawn = AIC->GetPawn();
	if (!IsValid(SelfPawn))
	{
		AIC->StopMovement();
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		AIC->StopMovement();
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	const FName KeyName = TargetActorKey.SelectedKeyName;
	// AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(KeyName));
	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(FSeasonBossAIKeys::NearestHouseActor));

	if (!IsValid(TargetActor))
	{
		AActor* PlayerPawn = UGameplayStatics::GetPlayerPawn(OwnerComp.GetWorld(), 0);
		if (IsValid(PlayerPawn))
		{
			TargetActor = PlayerPawn;
			CR4S_Log(LogDa, Warning,
				TEXT("[MoveToTargetDynamic] TickTask: Blackboard Key='%s' invalid → Fallback to PlayerPawn ptr=%p (Name='%s')"),
				*KeyName.ToString(),
				PlayerPawn,
				*PlayerPawn->GetName()
			);
		}
		else
		{
			CR4S_Log(LogDa, Warning,
				TEXT("[MoveToTargetDynamic] TickTask: Neither Blackboard Key='%s' nor PlayerPawn is valid → Fail"),
				*KeyName.ToString()
			);
			AIC->StopMovement();
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
			return;
		}
	}

	if (UMonsterStateComponent* StateComp = SelfPawn->FindComponentByClass<UMonsterStateComponent>())
	{
		StateComp->SetState(MovementState);
	}
	
	const float Dist = FVector::Dist(SelfPawn->GetActorLocation(), TargetActor->GetActorLocation());
	CR4S_Log(LogDa, Warning, TEXT("[ChasePlayerDynamic] Actual Distance = %.2f cm, AcceptanceRadius = %.2f cm"), Dist, AcceptanceRadius);
	if (Dist <= AcceptanceRadius)
	{
		AIC->StopMovement();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
}
