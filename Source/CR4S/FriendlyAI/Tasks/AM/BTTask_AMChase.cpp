#include "BTTask_AMChase.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "../../BaseAnimal.h"
#include "Components/SphereComponent.h"
#include "../../Component/AIJumpComponent.h"
#include "../../Controller/AnimalMonsterAIController.h"
#include "FriendlyAI/AnimalMonster.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_AMChase::UBTTask_AMChase()
{
	NodeName = TEXT("AMChase");
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_AMChase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{	
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller) return EBTNodeResult::Failed;

	ABaseAnimal* Animal = Cast<ABaseAnimal>(Controller->GetPawn());
	if (!Animal) return EBTNodeResult::Failed;
	
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	AActor* PlayerActor = Cast<AActor>(
		BB->GetValueAsObject("Player"));
	if (!PlayerActor) return EBTNodeResult::Failed;

	AActor* TargetActor = PlayerActor;

	TArray<FHitResult> Hits;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Animal);
	Params.AddIgnoredActor(PlayerActor);
	bool bHitHouse = GetWorld()->LineTraceMultiByChannel(
		Hits,
		Animal->GetActorLocation(),
		PlayerActor->GetActorLocation(),
		ECC_Visibility,
		Params);
	for (const FHitResult& Hit : Hits)
	{
		if (Hit.GetActor() && Hit.GetActor()->ActorHasTag("House"))
		{
			TargetActor = Hit.GetActor();
			break;
		}
	}
	    
	if (TargetActor->ActorHasTag("House"))
	{
	}
	else
	{
		if (ABaseAnimal* TargetAnimal = Cast<ABaseAnimal>(TargetActor))
		{
			if (TargetAnimal->CurrentState == EAnimalState::Dead)
			{
				if (AAnimalMonsterAIController* C = Cast<AAnimalMonsterAIController>(OwnerComp.GetAIOwner()))
				{
					if (ABaseAnimal* ControlledAnimal = Cast<ABaseAnimal>(C->GetPawn()))
					{
						if (ControlledAnimal->BehaviorTypeEnum == EAnimalBehavior::Aggressive)
						{
							C->SetAnimalState(EAnimalState::Chase);
						}
					}
				}
				return EBTNodeResult::Failed;
			}
		}
	}
    
	TargetToChase = TargetActor;

	if (AAnimalMonsterAIController* C = Cast<AAnimalMonsterAIController>(OwnerComp.GetAIOwner()))
	{
		C->SetTargetActor(TargetToChase);
	}

	const FVector SelfLocation   = Animal->GetActorLocation();
	const FVector TargetLocation = TargetToChase->GetActorLocation();
	const float Distance         = FVector::Dist(SelfLocation, TargetLocation);

	FAIMoveRequest MoveReq;
	MoveReq.SetGoalLocation(TargetLocation);
	
	float DesiredRadius = 0.f;
	if (Animal->bCanRanged && !Animal->bIsRangedOnCooldown)
	{
		DesiredRadius = Animal->RangedRange;
	}
	else if (Animal->bCanCharge && !Animal->bIsChargeOnCooldown)
	{
		DesiredRadius = Animal->DashRange;
	}
	else
	{
		DesiredRadius = Animal->MeleeRange;
	}
	
	MoveReq.SetAcceptanceRadius(DesiredRadius);

	if (AAnimalMonster* AnimalMonster = Cast<AAnimalMonster>(Animal))
	{
		if (AnimalMonster->AIJumpComponent)
		{
			AnimalMonster->AIJumpComponent->OnJumpFailed.Unbind();
			AnimalMonster->AIJumpComponent->OnJumpFailed.BindLambda([&OwnerComp]()
			{
				if (AAnimalMonsterAIController* C = Cast<AAnimalMonsterAIController>(OwnerComp.GetAIOwner()))
				{
					TArray<AActor*> FoundActors;
					UGameplayStatics::GetAllActorsWithTag(C->GetWorld(), FName("House"), FoundActors);
					if (FoundActors.Num() > 0)
					{
						C->SetTargetActor(FoundActors[0]);
					}
					C->SetAnimalState(EAnimalState::Attack);
				}
			});
			// AnimalMonster->AIJumpComponent->Activate();

			////
			if (AnimalMonster && AnimalMonster->GetCharacterMovement())
			{
				AnimalMonster->GetCharacterMovement()->bUseRVOAvoidance = true;
				AnimalMonster->GetCharacterMovement()->AvoidanceWeight = 0.5f;
			}
			if (BB)
			{
				AActor* Player = Cast<AActor>(BB->GetValueAsObject("Player"));
				if (Player)
				{
					if (AAnimalMonsterAIController* C = Cast<AAnimalMonsterAIController>(OwnerComp.GetAIOwner()))
					{
						C->MoveToActor(Player);
					}
				}
			}
			///
		}
	}
	return EBTNodeResult::InProgress;
}

void UBTTask_AMChase::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{	
	ABaseAnimal* Animal = Cast<ABaseAnimal>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Animal || Animal->CurrentState != EAnimalState::Chase || Animal->CurrentState == EAnimalState::Stun)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	ABaseAnimal* TargetAnimal = Cast<ABaseAnimal>(TargetToChase);
	if (!IsValid(TargetToChase) || (TargetAnimal && TargetAnimal->CurrentState == EAnimalState::Dead))
	{
		if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
		{
			BB->ClearValue(TEXT("TargetActor"));
		}
		Animal->ClearTarget();
		if (AAnimalMonsterAIController* C = Cast<AAnimalMonsterAIController>(OwnerComp.GetAIOwner()))
		{
			C->SetAnimalState(EAnimalState::Chase);
		}
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	static FVector LastTargetLocation = FVector::ZeroVector;
	const FVector CurrentTargetLocation = TargetToChase->GetActorLocation();
	const float DistanceMoved = FVector::DistSquared(LastTargetLocation, CurrentTargetLocation);
	
	if (DistanceMoved > FMath::Square(50.f))
	{
		LastTargetLocation = CurrentTargetLocation;
	}

	float Distance = FVector::Dist(
		Animal->GetActorLocation(),
		TargetToChase->GetActorLocation()
	);
	
	float AttackRange = Animal->MeleeRange;
	if (Animal->bCanRanged && !Animal->bIsRangedOnCooldown)
	{
		AttackRange = Animal->RangedRange;
	}
	else if (Animal->bCanCharge && !Animal->bIsChargeOnCooldown)
	{
		AttackRange = Animal->DashRange;
	}

	if (Distance <= AttackRange)
	{
		if (AAnimalMonsterAIController* C = Cast<AAnimalMonsterAIController>(OwnerComp.GetAIOwner()))
		{
			C->SetAnimalState(EAnimalState::Attack);
		}
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
}
