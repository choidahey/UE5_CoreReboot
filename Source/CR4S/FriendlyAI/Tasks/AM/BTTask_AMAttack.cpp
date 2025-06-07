#include "BTTask_AMAttack.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "../../Controller/AnimalMonsterAIController.h"
#include "../../BaseAnimal.h"
#include "Components/SphereComponent.h"

UBTTask_AMAttack::UBTTask_AMAttack()
{
	NodeName = TEXT("AM Attack");
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_AMAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	StoredOwnerComp = &OwnerComp;
	ABaseAnimal* Animal = Cast<ABaseAnimal>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Animal || !Animal->MeleeAttackMontage || !Animal->GetCurrentStats().AttackInterval)
		return EBTNodeResult::Failed;
	
	AActor* Target = Animal->CurrentTarget;
	//
	// if (!Target)
	// {
	// 	if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
	// 	{
	// 		Target = Cast<AActor>(BB->GetValueAsObject("TargetActor"));
	// 		if (Target) Animal->CurrentTarget = Target;
	// 	}
	// }
	//
	
	if (!Animal->AttackRange || !Target) return EBTNodeResult::Failed;

	const float Distance = FVector::Dist(Animal->GetActorLocation(), Target->GetActorLocation());
	float RandValue = FMath::FRand();

	if (Animal->CurrentState != EAnimalState::Attack ||
	(!Animal->bCanMelee && !Animal->bCanCharge && !Animal->bCanRanged))
	{
		if (AAnimalMonsterAIController* C = Cast<AAnimalMonsterAIController>(Animal->GetController()))
		{
			C->SetAnimalState(EAnimalState::Chase);
		}
		FinishLatentTask(*StoredOwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}

	if (Target->ActorHasTag("House"))
	{
		float Duration = Animal->PlayAttackMontage();
		Animal->GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, this, &UBTTask_AMAttack::OnAttackFinished, Duration, false);
		return EBTNodeResult::InProgress;
	}
	
     	if (Distance <= Animal->RangedRange && Animal->bCanRanged && !Animal->bIsRangedOnCooldown && RandValue < Animal->RangedProbability)
     	{
     		float Duration = Animal->PlayRangedAttackMontage();
     		Animal->GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, this, &UBTTask_AMAttack::OnAttackFinished, Duration, false);
     		return EBTNodeResult::InProgress;
     	}
     	else if (Distance <= Animal->DashRange && Animal->bCanCharge && !Animal->bIsChargeOnCooldown && RandValue < Animal->RangedProbability + Animal->ChargeProbability)
     	{
     		float Duration = Animal->PlayChargeAttackMontage();
     		Animal->GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, this, &UBTTask_AMAttack::OnAttackFinished, Duration, false);
     		return EBTNodeResult::InProgress;
     	}
     	else if (Distance <= Animal->MeleeRange && Animal->bCanMelee && !Animal->bIsMeleeOnCooldown && RandValue < Animal->MeleeProbability)
     	{
     		float Duration = Animal->PlayAttackMontage();
     		Animal->GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, this, &UBTTask_AMAttack::OnAttackFinished, Duration, false);
     		return EBTNodeResult::InProgress;
     	}
     	else
     	{
     		if (AAnimalMonsterAIController* C = Cast<AAnimalMonsterAIController>(Animal->GetController()))
     		{
     			C->SetAnimalState(EAnimalState::Chase);
     		}
     		FinishLatentTask(*StoredOwnerComp, EBTNodeResult::Failed);
     		return EBTNodeResult::Failed;
     	}
}

void UBTTask_AMAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAnimalMonsterAIController* Controller = Cast<AAnimalMonsterAIController>(OwnerComp.GetAIOwner());
	ABaseAnimal* Animal = Controller ? Cast<ABaseAnimal>(Controller->GetPawn()) : nullptr;
	if (!Animal || !IsValid(Animal->CurrentTarget) || Animal->CurrentState == EAnimalState::Stun)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	const float Distance = FVector::Dist(Animal->GetActorLocation(), Animal->CurrentTarget->GetActorLocation());
	const float LostRange = Animal->GetCurrentStats().TargetLostRange;

	if (!IsValid(Animal->CurrentTarget) || Animal->CurrentState == EAnimalState::Dead || Distance > LostRange)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
}

void UBTTask_AMAttack::OnAttackFinished()
{
	if (UBehaviorTreeComponent* OwnerComp = StoredOwnerComp.Get())
	{
		if (bCanFollowUp)
		{
			if (AAnimalMonsterAIController* C = Cast<AAnimalMonsterAIController>(StoredOwnerComp->GetAIOwner()))
			{
				if (ABaseAnimal* Animal = Cast<ABaseAnimal>(C->GetPawn()))
				{
					if (IsValid(Animal->CurrentTarget))
					{
						C->SetTargetActor(Animal->CurrentTarget);
						C->SetAnimalState(EAnimalState::Chase);
					}
				}
			}
		}
		FinishLatentTask(*StoredOwnerComp, EBTNodeResult::Succeeded);
	}
}
