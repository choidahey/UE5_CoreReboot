#include "BTTask_HelperBotAttack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "../BaseHelperBot.h"
#include "FriendlyAI/AnimalMonster.h"
#include "MonsterAI/BossMonster/Season/SeasonBossMonster.h"
#include "Engine/World.h"

UBTTask_HelperBotAttack::UBTTask_HelperBotAttack()
{
	NodeName = TEXT("HelperBot Attack");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_HelperBotAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		return EBTNodeResult::Failed;
	}

	APawn* Helper = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!Helper)
	{
		return EBTNodeResult::Failed;
	}
	
	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(AttackTargetKey.SelectedKeyName));
	
	if (!IsValidMonsterTarget(TargetActor))
	{
		TargetActor = FindNearestMonster(Helper);
		if (!TargetActor)
		{
			return EBTNodeResult::Failed;
		}
		BB->SetValueAsObject(AttackTargetKey.SelectedKeyName, TargetActor);
	}
	
	float Distance = FVector::Dist(Helper->GetActorLocation(), TargetActor->GetActorLocation());
	if (Distance > AttackRange)
	{
		return EBTNodeResult::Failed;
	}

	CachedTarget = TargetActor;
	CachedHelper = Helper;
	
	CachedTarget->OnDestroyed.RemoveDynamic(this, &UBTTask_HelperBotAttack::OnTargetDestroyed);
	CachedTarget->OnDestroyed.AddDynamic(this, &UBTTask_HelperBotAttack::OnTargetDestroyed);
	
	ABaseHelperBot* HelperBot = Cast<ABaseHelperBot>(Helper);
	if (HelperBot)
	{
		CachedAttackPerSecond = HelperBot->GetAttackPerSecond();
		HelperBot->SetIsWorking(true);
		HelperBot->UpdateEyeBeamWorkTarget(TargetActor);
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_HelperBotAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!CachedHelper)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	ABaseHelperBot* HelperBot = Cast<ABaseHelperBot>(CachedHelper);
	if (!HelperBot)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	if (!IsValid(CachedTarget) || CachedTarget->IsActorBeingDestroyed())
	{
		CachedTarget = FindNearestMonster(CachedHelper);
		if (!CachedTarget)
		{
			HelperBot->SetIsWorking(false);
			HelperBot->StopEyeBeamWork();
			UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
			if (BB)
			{
				BB->ClearValue(AttackTargetKey.SelectedKeyName);
			}
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return;
		}
		
		CachedTarget->OnDestroyed.RemoveDynamic(this, &UBTTask_HelperBotAttack::OnTargetDestroyed);
		CachedTarget->OnDestroyed.AddDynamic(this, &UBTTask_HelperBotAttack::OnTargetDestroyed);
		
		UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
		if (BB)
		{
			BB->SetValueAsObject(AttackTargetKey.SelectedKeyName, CachedTarget);
		}
	}
	
	float Distance = FVector::Dist(CachedHelper->GetActorLocation(), CachedTarget->GetActorLocation());
	if (Distance > AttackRange)
	{
		CachedTarget = FindNearestMonster(CachedHelper);
		if (!CachedTarget)
		{
			HelperBot->SetIsWorking(false);
			HelperBot->StopEyeBeamWork();
			UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
			if (BB)
			{
				BB->ClearValue(AttackTargetKey.SelectedKeyName);
			}
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return;
		}
		
		CachedTarget->OnDestroyed.RemoveDynamic(this, &UBTTask_HelperBotAttack::OnTargetDestroyed);
		CachedTarget->OnDestroyed.AddDynamic(this, &UBTTask_HelperBotAttack::OnTargetDestroyed);
		
		UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
		if (BB)
		{
			BB->SetValueAsObject(AttackTargetKey.SelectedKeyName, CachedTarget);
		}
	}
	
	FVector TargetDirection = (CachedTarget->GetActorLocation() - CachedHelper->GetActorLocation()).GetSafeNormal();
	FRotator TargetRotation = TargetDirection.Rotation();
	FRotator CurrentRotation = CachedHelper->GetActorRotation();
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, 2.0f);
	CachedHelper->SetActorRotation(NewRotation);
	
	HelperBot->UpdateEyeBeamWorkTarget(CachedTarget);
	
	const float DamageThisFrame = CachedAttackPerSecond * DeltaSeconds;
	UGameplayStatics::ApplyDamage(CachedTarget, DamageThisFrame, HelperBot->GetController(), HelperBot, UDamageType::StaticClass());
}

void UBTTask_HelperBotAttack::OnTargetDestroyed(AActor* /*DestroyedActor*/)
{
	if (CachedTarget)
	{
		CachedTarget->OnDestroyed.RemoveDynamic(this, &UBTTask_HelperBotAttack::OnTargetDestroyed);
		CachedTarget = nullptr;
	}
}

AActor* UBTTask_HelperBotAttack::FindNearestMonster(APawn* Helper)
{
	if (!Helper || !Helper->GetWorld())
	{
		return nullptr;
	}

	TArray<AActor*> FoundMonsters;
	
	TArray<AActor*> AnimalMonsters;
	UGameplayStatics::GetAllActorsOfClass(Helper->GetWorld(), AAnimalMonster::StaticClass(), AnimalMonsters);
	FoundMonsters.Append(AnimalMonsters);
	
	TArray<AActor*> BossMonsters;
	UGameplayStatics::GetAllActorsOfClass(Helper->GetWorld(), ASeasonBossMonster::StaticClass(), BossMonsters);
	FoundMonsters.Append(BossMonsters);

	if (FoundMonsters.Num() == 0)
	{
		return nullptr;
	}
	
	AActor* NearestMonster = nullptr;
	float NearestDistance = AttackRange;
	FVector HelperLocation = Helper->GetActorLocation();

	for (AActor* Monster : FoundMonsters)
	{
		if (!IsValid(Monster))
		{
			continue;
		}

		float Distance = FVector::Dist(HelperLocation, Monster->GetActorLocation());
		if (Distance < NearestDistance)
		{
			NearestDistance = Distance;
			NearestMonster = Monster;
		}
	}

	return NearestMonster;
}

bool UBTTask_HelperBotAttack::IsValidMonsterTarget(AActor* Actor)
{
	if (!IsValid(Actor))
	{
		return false;
	}
	
	if (AAnimalMonster* AnimalMonster = Cast<AAnimalMonster>(Actor))
	{
		return AnimalMonster->CurrentState != EAnimalState::Dead;
	}
	
	if (ASeasonBossMonster* BossMonster = Cast<ASeasonBossMonster>(Actor))
	{
		return !BossMonster->IsDead();
	}

	return false;
}