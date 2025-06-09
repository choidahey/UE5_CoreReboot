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

float UBTTaskNode_MoveToTargetDynamic::GetCurrentAcceptanceRadius(const FName KeyName, UBlackboardComponent* BB) const
{
	if (!BB)
	{
		return DefaultAcceptanceRadius;
	}
	return BB->GetValueAsFloat(KeyName);
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
    	TargetActor = UGameplayStatics::GetPlayerPawn(OwnerComp.GetWorld(), 0);
    	if (!IsValid(TargetActor))
    	{
    		return EBTNodeResult::Failed;
    	}
    }

	float Radius = DefaultAcceptanceRadius;
	if (KeyName == FAIKeys::TargetActor)
	{
		Radius = GetCurrentAcceptanceRadius(PlayerAcceptanceRadiusKey.SelectedKeyName, BB);
	}
	else if (KeyName == FSeasonBossAIKeys::NearestHouseActor)
	{
		Radius = GetCurrentAcceptanceRadius(HouseAcceptanceRadiusKey.SelectedKeyName, BB);
	}
	
	EPathFollowingRequestResult::Type PFResult = AIC->MoveToActor(
		TargetActor,
		Radius,
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
	
	float Radius = DefaultAcceptanceRadius;
	if (KeyName == FAIKeys::TargetActor)
	{
		Radius = GetCurrentAcceptanceRadius(PlayerAcceptanceRadiusKey.SelectedKeyName, BB);
	}
	else if (KeyName == FSeasonBossAIKeys::NearestHouseActor)
	{
		Radius = GetCurrentAcceptanceRadius(HouseAcceptanceRadiusKey.SelectedKeyName, BB);
	}
	
	const float Dist = FVector::Dist(SelfPawn->GetActorLocation(), TargetActor->GetActorLocation());
	
	CR4S_Log(LogDa, Log,
		TEXT("[MoveToTargetDynamic] %s %s Dist = %.2f, Radius = %.2f"),
		*SelfPawn->GetName(),
		*TargetActor->GetName(),
		Dist,
		Radius
	);
	
	if (Dist <= Radius)
	{
		CR4S_Log(LogDa, Log,
			TEXT("[MoveToTargetDynamic] %s Arrived!"),
			*SelfPawn->GetName()
		);
		
		AIC->StopMovement();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
}
