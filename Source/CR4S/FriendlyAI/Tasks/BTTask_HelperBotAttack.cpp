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

uint16 UBTTask_HelperBotAttack::GetInstanceMemorySize() const
{
	return sizeof(FBTAttackMemory);
}

EBTNodeResult::Type UBTTask_HelperBotAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FBTAttackMemory* MyMemory = reinterpret_cast<FBTAttackMemory*>(NodeMemory);
	if (!MyMemory) return EBTNodeResult::Failed;

	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	APawn* Pawn = AICon->GetPawn();
	if (!Pawn) return EBTNodeResult::Failed;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	MyMemory->CachedHelper = Pawn;
	ABaseHelperBot* Helper = Cast<ABaseHelperBot>(Pawn);
	if (!Helper) return EBTNodeResult::Failed;

	AActor* TargetEnemy = FindNearestEnemy(Helper);
	if (!TargetEnemy) return EBTNodeResult::Failed;

	BB->SetValueAsObject(TEXT("AttackTarget"), TargetEnemy);
	MyMemory->CachedTarget = TargetEnemy;
	
	MyMemory->CachedDamagePerSecond = Helper->GetAttackPerSecond();
	Helper->SetIsWorking(true);
	Helper->UpdateEyeBeamWorkTarget(TargetEnemy);
	
	MyMemory->AttackTimer = 0.f;
	
	return EBTNodeResult::InProgress;
}

void UBTTask_HelperBotAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FBTAttackMemory* MyMemory = reinterpret_cast<FBTAttackMemory*>(NodeMemory);
	if (!MyMemory)
	{
		CleanupAndFinish(OwnerComp, NodeMemory, EBTNodeResult::Failed);
		return;
	}

	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon)
	{
		CleanupAndFinish(OwnerComp, NodeMemory, EBTNodeResult::Failed);
		return;
	}

	APawn* Pawn = AICon->GetPawn();
	if (!Pawn)
	{
		CleanupAndFinish(OwnerComp, NodeMemory, EBTNodeResult::Failed);
		return;
	}

	if (!MyMemory->CachedHelper.IsValid())
	{
		CleanupAndFinish(OwnerComp, NodeMemory, EBTNodeResult::Failed);
		return;
	}

	ABaseHelperBot* Helper = Cast<ABaseHelperBot>(MyMemory->CachedHelper.Get());
	if (!Helper)
	{
		CleanupAndFinish(OwnerComp, NodeMemory, EBTNodeResult::Failed);
		return;
	}

	if (MyMemory->CachedTarget.IsValid())
	{
		AActor* Target = MyMemory->CachedTarget.Get();
		FVector Direction = (Target->GetActorLocation() - Helper->GetActorLocation()).GetSafeNormal();
		FRotator LookRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
		Helper->SetActorRotation(LookRotation);

		Helper->UpdateEyeBeamWorkTarget(Target);
	}
	
	MyMemory->AttackTimer += DeltaSeconds;

	if (MyMemory->AttackTimer >= MaxAttackDuration)
	{
		CleanupAndFinish(OwnerComp, NodeMemory, EBTNodeResult::Succeeded);
		return;
	}
	
	if (!MyMemory->CachedTarget.IsValid())
	{
		CleanupAndFinish(OwnerComp, NodeMemory, EBTNodeResult::Succeeded);
		return;
	}

	AActor* Target = MyMemory->CachedTarget.Get();
	AAnimalMonster* Monster = Cast<AAnimalMonster>(Target);
	ASeasonBossMonster* Boss = Cast<ASeasonBossMonster>(Target);
	if (Target->IsActorBeingDestroyed() || 
		(Monster && Monster->CurrentState == EAnimalState::Dead) ||
		(Boss && Boss->IsDead()))
	{
		CleanupAndFinish(OwnerComp, NodeMemory, EBTNodeResult::Succeeded);
		return;
	}
	
	if (MyMemory->CachedTarget.IsValid())
	{
		float DistanceToTarget = FVector::Dist(Helper->GetActorLocation(), Target->GetActorLocation());
		if (DistanceToTarget > AttackRange)
		{
			CleanupAndFinish(OwnerComp, NodeMemory, EBTNodeResult::Succeeded);
			return;
		}
	}
	
	if (MyMemory->CachedTarget.IsValid())
	{
		const float DamageThisFrame = MyMemory->CachedDamagePerSecond * DeltaSeconds;
		UGameplayStatics::ApplyDamage(MyMemory->CachedTarget.Get(), DamageThisFrame, Helper->GetController(), Helper, UDamageType::StaticClass());
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

void UBTTask_HelperBotAttack::CleanupAndFinish(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type Result)
{
	FBTAttackMemory* MyMemory = reinterpret_cast<FBTAttackMemory*>(NodeMemory);
	if (!MyMemory)
	{
		FinishLatentTask(OwnerComp, Result);
		return;
	}

	ABaseHelperBot* Helper = Cast<ABaseHelperBot>(MyMemory->CachedHelper.Get());
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