#include "BTTask_HelperBotAttack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "../BaseHelperBot.h"
#include "../AnimalMonster.h"
#include "Engine/World.h"
#include "MonsterAI/BossMonster/Season/SeasonBossMonster.h"

UBTTask_HelperBotAttack::UBTTask_HelperBotAttack()
{
	NodeName = TEXT("HelperBotAttack");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_HelperBotAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	CachedHelper = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	ABaseHelperBot* Helper = Cast<ABaseHelperBot>(CachedHelper);
	if (!Helper) return EBTNodeResult::Failed;

	AActor* TargetEnemy = FindNearestEnemy(Helper);
	if (!TargetEnemy) return EBTNodeResult::Failed;

	BB->SetValueAsObject(TEXT("AttackTarget"), TargetEnemy);
	CachedTarget = TargetEnemy;
	
	CachedDamagePerSecond = Helper->GetAttackPerSecond();
	Helper->SetIsWorking(true);
	Helper->UpdateEyeBeamWorkTarget(TargetEnemy);
	
	AttackTimer = 0.f;
	
	return EBTNodeResult::InProgress;
}

void UBTTask_HelperBotAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!CachedHelper)
	{
		CleanupAndFinish(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	ABaseHelperBot* Helper = Cast<ABaseHelperBot>(CachedHelper);
	if (!Helper)
	{
		CleanupAndFinish(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (CachedTarget && IsValid(CachedTarget))
	{
		FVector Direction = (CachedTarget->GetActorLocation() - Helper->GetActorLocation()).GetSafeNormal();
		FRotator LookRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
		Helper->SetActorRotation(LookRotation);

		Helper->UpdateEyeBeamWorkTarget(CachedTarget);
	}
	
	AttackTimer += DeltaSeconds;

	if (AttackTimer >= MaxAttackDuration)
	{
		CleanupAndFinish(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
	
	AAnimalMonster* Monster = Cast<AAnimalMonster>(CachedTarget);
	ASeasonBossMonster* Boss = Cast<ASeasonBossMonster>(CachedTarget);
	if (!IsValid(CachedTarget) || CachedTarget->IsActorBeingDestroyed() || 
		(Monster && Monster->CurrentState == EAnimalState::Dead) ||
		(Boss && Boss->IsDead())) 
	{
		CleanupAndFinish(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
	
	if (CachedTarget)
	{
		float DistanceToTarget = FVector::Dist(Helper->GetActorLocation(), CachedTarget->GetActorLocation());
		if (DistanceToTarget > AttackRange)
		{
			CleanupAndFinish(OwnerComp, EBTNodeResult::Succeeded);
			return;
		}
	}
	
	if (CachedTarget)
	{
		const float DamageThisFrame = CachedDamagePerSecond * DeltaSeconds;
		UGameplayStatics::ApplyDamage(CachedTarget, DamageThisFrame, Helper->GetController(), Helper, UDamageType::StaticClass());
	}
}

AActor* UBTTask_HelperBotAttack::FindNearestEnemy(APawn* Helper)
{
	if (!Helper) return nullptr;
	
	UWorld* World = Helper->GetWorld();
	if (!World) return nullptr;
	
	TArray<AActor*> FoundEnemies;
	UGameplayStatics::GetAllActorsOfClass(World, AAnimalMonster::StaticClass(), FoundEnemies);

	TArray<AActor*> BossEnemies;
	UGameplayStatics::GetAllActorsOfClass(World, ASeasonBossMonster::StaticClass(), BossEnemies);
	FoundEnemies.Append(BossEnemies);
	
	AActor* NearestEnemy = nullptr;
	float NearestDistance = AttackRange;
	FVector HelperLocation = Helper->GetActorLocation();
	
	for (AActor* Enemy : FoundEnemies)
	{
		if (!IsValid(Enemy) || Enemy->IsActorBeingDestroyed()) continue;
		
		AAnimalMonster* Monster = Cast<AAnimalMonster>(Enemy);
		if (Monster && Monster->CurrentState == EAnimalState::Dead) continue;

		ASeasonBossMonster* Boss = Cast<ASeasonBossMonster>(Enemy);
		if (Boss && Boss->IsDead()) continue;
		
		float Distance = FVector::Dist(HelperLocation, Enemy->GetActorLocation());
		if (Distance <= AttackRange && Distance < NearestDistance)
		{
			NearestDistance = Distance;
			NearestEnemy = Enemy;
		}
	}
	
	return NearestEnemy;
}

void UBTTask_HelperBotAttack::CleanupAndFinish(UBehaviorTreeComponent& OwnerComp, EBTNodeResult::Type Result)
{
	ABaseHelperBot* Helper = Cast<ABaseHelperBot>(CachedHelper);
	if (Helper)
	{
		Helper->SetIsWorking(false);
		Helper->StopEyeBeamWork();
	}
	
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (BB)
	{
		BB->SetValueAsObject(TEXT("AttackTarget"), nullptr);
	}
	
	FinishLatentTask(OwnerComp, Result);
}