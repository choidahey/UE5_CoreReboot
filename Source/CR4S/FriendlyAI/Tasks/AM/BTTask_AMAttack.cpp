#include "BTTask_AMAttack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "FriendlyAI/Controller/AnimalMonsterAIController.h"
#include "FriendlyAI/AnimalMonster.h"

UBTTask_AMAttack::UBTTask_AMAttack()
{
	NodeName = TEXT("AM Attack");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_AMAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAnimalMonsterAIController* AIController = Cast<AAnimalMonsterAIController>(OwnerComp.GetAIOwner());
	if (!IsValid(AIController))
	{
		return EBTNodeResult::Failed;
	}

	AAnimalMonster* Monster = Cast<AAnimalMonster>(AIController->GetPawn());
	if (!IsValid(Monster) || !IsValid(Monster->CurrentTarget))
	{
		return EBTNodeResult::Failed;
	}
	
	float DistanceToTarget = FVector::Dist(Monster->GetActorLocation(), Monster->CurrentTarget->GetActorLocation());
	
	EAttackType SelectedAttack = SelectAttackType(Monster, DistanceToTarget);
	
	if (SelectedAttack == EAttackType::None)
	{
		return EBTNodeResult::Failed;
	}
	
	ExecuteAttack(Monster, SelectedAttack);
	
	return EBTNodeResult::InProgress;
}

void UBTTask_AMAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AAnimalMonster* Monster = Cast<AAnimalMonster>(OwnerComp.GetAIOwner()->GetPawn());
	if (!IsValid(Monster))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	if (!Monster->bIsAttacking)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

EAttackType UBTTask_AMAttack::SelectAttackType(AAnimalMonster* Monster, float DistanceToTarget)
{
	if (!IsValid(Monster)) return EAttackType::None;
	
	TArray<EAttackType> AvailableAttacks = GetAvailableAttacks(Monster, DistanceToTarget);
	
	if (AvailableAttacks.Num() == 0)
	{
		return EAttackType::None;
	}
	
	if (AvailableAttacks.Num() == 1)
	{
		return AvailableAttacks[0];
	}
	
	TArray<float> Weights;
	for (EAttackType AttackType : AvailableAttacks)
	{
		switch (AttackType)
		{
		case EAttackType::Melee:
			Weights.Add(Monster->MeleeProbability);
			break;
		case EAttackType::Charge:
			Weights.Add(Monster->ChargeProbability);
			break;
		case EAttackType::Ranged:
			Weights.Add(Monster->RangedProbability);
			break;
		default:
			Weights.Add(0.0f);
			break;
		}
	}
	
	float TotalWeight = 0.0f;
	for (float Weight : Weights)
	{
		TotalWeight += Weight;
	}

	if (TotalWeight <= 0.0f)
	{
		return AvailableAttacks[0];
	}
	
	float RandomValue = FMath::FRandRange(0.0f, TotalWeight);
	float CurrentWeight = 0.0f;
	
	for (int32 i = 0; i < AvailableAttacks.Num(); ++i)
	{
		CurrentWeight += Weights[i];
		if (RandomValue <= CurrentWeight)
		{
			return AvailableAttacks[i];
		}
	}
	
	return AvailableAttacks[0];
}

TArray<EAttackType> UBTTask_AMAttack::GetAvailableAttacks(AAnimalMonster* Monster, float DistanceToTarget)
{
	TArray<EAttackType> AvailableAttacks;
	
	if (!IsValid(Monster)) return AvailableAttacks;
	
	if (CanPerformAttack(Monster, EAttackType::Melee, DistanceToTarget))
	{
		AvailableAttacks.Add(EAttackType::Melee);
	}
	
	if (CanPerformAttack(Monster, EAttackType::Charge, DistanceToTarget))
	{
		AvailableAttacks.Add(EAttackType::Charge);
	}
	
	if (CanPerformAttack(Monster, EAttackType::Ranged, DistanceToTarget))
	{
		AvailableAttacks.Add(EAttackType::Ranged);
	}

	return AvailableAttacks;
}

bool UBTTask_AMAttack::CanPerformAttack(AAnimalMonster* Monster, EAttackType AttackType, float DistanceToTarget)
{
	if (!IsValid(Monster)) return false;

	switch (AttackType)
	{
	case EAttackType::Melee:
		return Monster->bCanMelee && 
		       !Monster->bIsMeleeOnCooldown && 
		       DistanceToTarget <= Monster->MeleeRange;
		       
	case EAttackType::Charge:
		return Monster->bCanCharge && 
		       !Monster->bIsChargeOnCooldown && 
		       DistanceToTarget <= Monster->DashRange;
		       
	case EAttackType::Ranged:
		return Monster->bCanRanged && 
		       !Monster->bIsRangedOnCooldown && 
		       DistanceToTarget <= Monster->RangedRange;
		       
	default:
		return false;
	}
}

void UBTTask_AMAttack::ExecuteAttack(AAnimalMonster* Monster, EAttackType AttackType)
{
	switch (AttackType)
	{
	case EAttackType::Melee:
		Monster->PlayAttackMontage();
		break;
	case EAttackType::Charge:
		Monster->PlayChargeAttackMontage();
		break;
	case EAttackType::Ranged:
		Monster->PlayRangedAttackMontage();
		break;
	}
}