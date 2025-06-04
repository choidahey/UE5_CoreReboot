#include "BaseMonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "CR4S/MonsterAI/BaseMonster.h"
#include "CR4S/MonsterAI/Components/MonsterPerceptionComponent.h"
#include "CR4S/MonsterAI/Components/MonsterAttributeComponent.h"
#include "CR4S/MonsterAI/Data/MonsterAIKeyNames.h"

ABaseMonsterAIController::ABaseMonsterAIController()
	: MyHeader(TEXT("BaseMonsterAIController"))
{
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComp"));
	PerceptionComp = CreateDefaultSubobject<UMonsterPerceptionComponent>(TEXT("PerceptionComp"));

	SetPerceptionComponent(*PerceptionComp);
}

void ABaseMonsterAIController::BeginPlay()
{
	Super::BeginPlay();

}

void ABaseMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (const ABaseMonster* Monster = Cast<ABaseMonster>(InPawn))
	{
		if (PerceptionComp)
		{
			PerceptionComp->OnPlayerDetected.AddDynamic(this, &ABaseMonsterAIController::OnTargetDetected);
			PerceptionComp->OnPlayerLost.AddDynamic(this, &ABaseMonsterAIController::OnTargetLost);
		}

		if (UBehaviorTree* BT = Monster->BehaviorTree)
		{
			UBlackboardComponent* RawBBComp = BlackboardComp.Get();
			UseBlackboard(BT->BlackboardAsset, RawBBComp);
			RunBehaviorTree(BT);
		}
	}
}

void ABaseMonsterAIController::SetupPerceptionFromMonster(ABaseMonster* Monster)
{
	if (UMonsterAttributeComponent* AttributeComp = Monster->FindComponentByClass<UMonsterAttributeComponent>())
	{
		PerceptionComp->ApplySightConfigFromAttribute(AttributeComp->GetMonsterAttribute());
		UE_LOG(LogTemp, Log, TEXT("[%s] SetupPerceptionFromMonster"), *MyHeader);
	}
}

void ABaseMonsterAIController::StartChasingPlayer(AActor* Target)
{
	if (Target && BlackboardComp)
	{
		BlackboardComp->SetValueAsObject(FAIKeys::TargetActor, Target);
		BlackboardComp->SetValueAsBool(FAIKeys::CanSeePlayer, true);
		UE_LOG(LogTemp, Log, TEXT("[%s] StartChasingPlayer"), *MyHeader);
	}
}

void ABaseMonsterAIController::StopChasingPlayer()
{
	if (BlackboardComp)
	{
		BlackboardComp->ClearValue(FAIKeys::TargetActor);
		BlackboardComp->SetValueAsBool(FAIKeys::CanSeePlayer, false);
		UE_LOG(LogTemp, Log, TEXT("[%s] StopChasingPlayer"), *MyHeader);
	}
}

void ABaseMonsterAIController::StartSearchingLastLocation()
{
	if (BlackboardComp && GetPawn())
	{
		const FVector LastSeen = BlackboardComp->GetValueAsVector(FAIKeys::LastSeenLocation);
		MoveToLocation(LastSeen);
		UE_LOG(LogTemp, Log, TEXT("[%s] StartSearchingLastLocation"), *MyHeader);

	}
}

void ABaseMonsterAIController::OnTargetDetected(AActor* DetectedActor)
{
	if (!DetectedActor || !BlackboardComp)
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] OnTargetDetected : DetectedActor or BlackboardComp is Null"), *MyHeader);
		return;
	}

	BlackboardComp->SetValueAsObject(FAIKeys::TargetActor, DetectedActor);
	BlackboardComp->SetValueAsBool(FAIKeys::CanSeePlayer, true);
	BlackboardComp->SetValueAsVector(FAIKeys::LastSeenLocation, DetectedActor->GetActorLocation());

}

void ABaseMonsterAIController::OnTargetLost(AActor* LostActor)
{
	if (!BlackboardComp)
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] OnTargetLost : BlackboardComp is Null"), *MyHeader);
		return;
	}

	BlackboardComp->SetValueAsBool(FAIKeys::CanSeePlayer, false);

	if (BlackboardComp->GetValueAsObject(FAIKeys::TargetActor) == LostActor)
	{
		BlackboardComp->SetValueAsVector(FAIKeys::LastSeenLocation, LostActor->GetActorLocation());
		BlackboardComp->ClearValue(FAIKeys::TargetActor);
	}

	UE_LOG(LogTemp, Log, TEXT("[%s] OnTargetLost : Lost target."), *MyHeader);
}
