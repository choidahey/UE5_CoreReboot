#include "BTTask_AnimalAttack.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "../Controller/AnimalAIController.h"
#include "../BaseAnimal.h"
#include "Components/SphereComponent.h"

UBTTask_AnimalAttack::UBTTask_AnimalAttack()
{
	NodeName = TEXT("AnimalAttack");
}

EBTNodeResult::Type UBTTask_AnimalAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	StoredOwnerComp = &OwnerComp;
	ABaseAnimal* Animal = Cast<ABaseAnimal>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Animal || !Animal->AttackMontage || !Animal->GetCurrentStats().AttackInterval)
		return EBTNodeResult::Failed;

	if (!Animal->AttackRange || !Animal->CurrentTarget) return EBTNodeResult::Failed;
	if (!Animal->AttackRange->IsOverlappingActor(Animal->CurrentTarget)) return EBTNodeResult::Failed;

	UAnimInstance* Anim = Animal->GetMesh()->GetAnimInstance();
	float MontageDuration = Anim->Montage_Play(Animal->AttackMontage);
	float TotalDelay      = MontageDuration + Animal->GetCurrentStats().AttackInterval;

	FTimerDelegate FinishDel = FTimerDelegate::CreateUObject(
		this, &UBTTask_AnimalAttack::OnAttackFinished);
	OwnerComp.GetWorld()->GetTimerManager()
		.SetTimer(AttackTimerHandle, FinishDel, TotalDelay, false);

	return EBTNodeResult::InProgress;
}

void UBTTask_AnimalAttack::OnAttackFinished()
{
	if (UBehaviorTreeComponent* OwnerComp = StoredOwnerComp.Get())
	{
		if (AAnimalAIController* C = Cast<AAnimalAIController>(OwnerComp->GetAIOwner()))
		{
			C->SetAnimalState(EAnimalState::Chase);
		}
		FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
	}
}
