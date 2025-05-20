#include "BTTask_AnimalAttack.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "../BaseAnimal.h"

UBTTask_AnimalAttack::UBTTask_AnimalAttack()
{
	NodeName = TEXT("AnimalAttack");
}

EBTNodeResult::Type UBTTask_AnimalAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	bNotifyTick = true;

	ABaseAnimal* Animal = Cast<ABaseAnimal>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Animal || !Animal->AttackMontage || !Animal->CurrentTarget) return EBTNodeResult::Failed;

	Animal->PlayAttackMontage();

	return EBTNodeResult::InProgress;
}

void UBTTask_AnimalAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ABaseAnimal* Animal = Cast<ABaseAnimal>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Animal || Animal->CurrentState == EAnimalState::Dead)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (ABaseAnimal* TargetAnimal = Cast<ABaseAnimal>(Animal->CurrentTarget))
	{
		if (TargetAnimal->CurrentState == EAnimalState::Dead)
		{
			Animal->ClearTarget();
			OwnerComp.GetBlackboardComponent()->ClearValue(TEXT("TargetActor"));
			Animal->SetAnimalState(EAnimalState::Patrol);
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
			return;
		}
	}

	UAnimInstance* Anim = Animal->GetMesh()->GetAnimInstance();
	if (!Anim || !Anim->Montage_IsPlaying(Animal->AttackMontage))
	{
		Animal->SetAnimalState(EAnimalState::Chase);
		Animal->bIsAttacking = false;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
