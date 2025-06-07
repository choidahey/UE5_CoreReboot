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
    if (!IsValid(AIC)) return EBTNodeResult::Failed;
	
	APawn* OwnerPawn = AIC->GetPawn();
	if (!IsValid(OwnerPawn)) return EBTNodeResult::Failed;

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!IsValid(BB)) return EBTNodeResult::Failed;
	
	const FName KeyName = TargetActorKey.SelectedKeyName;
	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(KeyName));
	
    if (!IsValid(TargetActor))
    {
    	AActor* PlayerPawn = UGameplayStatics::GetPlayerPawn(OwnerComp.GetWorld(), 0);
    	
    	if (IsValid(PlayerPawn))
    		TargetActor = PlayerPawn;
    	else
    		return EBTNodeResult::Failed;
    }

	if (UMonsterStateComponent* StateComp = OwnerPawn->FindComponentByClass<UMonsterStateComponent>())
		StateComp->SetState(MovementState);
	
	EPathFollowingRequestResult::Type PFResult = AIC->MoveToActor(
		TargetActor,
		AcceptanceRadius,
		false,
		true,
		true,
		nullptr,
		true
	);
	
	return EBTNodeResult::InProgress;
}

void UBTTaskNode_MoveToTargetDynamic::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!IsValid(AIC))
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
	if (!IsValid(BB))
	{
		AIC->StopMovement();
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	const FName KeyName = TargetActorKey.SelectedKeyName;
	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(KeyName));

	if (!IsValid(TargetActor))
	{
		AActor* PlayerPawn = UGameplayStatics::GetPlayerPawn(OwnerComp.GetWorld(), 0);
		if (IsValid(PlayerPawn))
		{
			TargetActor = PlayerPawn;
		}
		else
		{
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
	if (Dist <= AcceptanceRadius)
	{
		AIC->StopMovement();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
}
