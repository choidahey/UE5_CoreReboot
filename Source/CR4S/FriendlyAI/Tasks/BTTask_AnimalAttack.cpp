#include "BTTask_AnimalAttack.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "../Controller/AnimalAIController.h"
#include "../BaseAnimal.h"
#include "Components/SphereComponent.h"

UBTTask_AnimalAttack::UBTTask_AnimalAttack()
{
	NodeName = TEXT("Animal Attack");
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_AnimalAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	StoredOwnerComp = &OwnerComp;
	ABaseAnimal* Animal = Cast<ABaseAnimal>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Animal || !Animal->AttackMontage || !Animal->GetCurrentStats().AttackInterval)
		return EBTNodeResult::Failed;

	if (!Animal->AttackRange || !Animal->CurrentTarget) return EBTNodeResult::Failed;
	
	const float TimeNow = Animal->GetWorld()->GetTimeSeconds();
	if (!Animal->AttackRange->IsOverlappingActor(Animal->CurrentTarget)
		|| (TimeNow - Animal->LastAttackTime < Animal->CachedAttackInterval))
	{
		if (AAnimalAIController* C = Cast<AAnimalAIController>(Animal->GetController()))
		{
			C->SetTargetActor(Animal->CurrentTarget);
			C->SetAnimalState(EAnimalState::Chase);
		}
		return EBTNodeResult::Succeeded;
	}
	
	UAnimInstance* Anim = Animal->GetMesh()->GetAnimInstance();
	Animal->LastAttackTime = TimeNow;
	float MontageDuration = Anim->Montage_Play(Animal->AttackMontage);
	float TotalDelay = MontageDuration + Animal->GetCurrentStats().AttackInterval;
	
	if (ABaseAnimal* TargetAnimal = Cast<ABaseAnimal>(Animal->CurrentTarget))
	{
		if (TargetAnimal->CurrentState == EAnimalState::Dead)
		{
			Animal->ClearTarget();
			if (AAnimalAIController* C = Cast<AAnimalAIController>(Animal->GetController()))
			{
				C->SetAnimalState(EAnimalState::Patrol);
			}
			bCanFollowUp = false;
		}
	}
	else if (!Animal->AttackRange->IsOverlappingActor(Animal->CurrentTarget))
	{
		bCanFollowUp = true;
	}
	
	FTimerDelegate FinishDel = FTimerDelegate::CreateUObject(
		this, &UBTTask_AnimalAttack::OnAttackFinished);
	OwnerComp.GetWorld()->GetTimerManager()
		.SetTimer(AttackTimerHandle, FinishDel, TotalDelay, false);

	return EBTNodeResult::InProgress;
}

void UBTTask_AnimalAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAnimalAIController* Controller = Cast<AAnimalAIController>(OwnerComp.GetAIOwner());
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

void UBTTask_AnimalAttack::OnAttackFinished()
{
	if (UBehaviorTreeComponent* OwnerComp = StoredOwnerComp.Get())
	{
		if (bCanFollowUp)
		{
			if (AAnimalAIController* C = Cast<AAnimalAIController>(StoredOwnerComp->GetAIOwner()))
			{
				if (ABaseAnimal* Animal = Cast<ABaseAnimal>(C->GetPawn()))
				{
					if (IsValid(Animal->CurrentTarget))
					{
						C->SetTargetActor(Animal->CurrentTarget);
						C->SetAnimalState(EAnimalState::Chase);
					}
					else
					{
						C->SetAnimalState(EAnimalState::Patrol);
					}
				}
			}
		}
		FinishLatentTask(*StoredOwnerComp, EBTNodeResult::Succeeded);
	}
}
