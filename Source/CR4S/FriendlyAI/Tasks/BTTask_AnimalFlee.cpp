#include "BTTask_AnimalFlee.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"
#include "../BaseAnimal.h"
#include "Character/Characters/ModularRobot.h"
#include "Character/Characters/PlayerCharacter.h"
#include "FriendlyAI/AnimalMonster.h"
#include "FriendlyAI/Controller/AnimalAIController.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/BaseMonster.h"

UBTTask_AnimalFlee::UBTTask_AnimalFlee()
{
	NodeName = TEXT("AnimalFlee");
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

uint16 UBTTask_AnimalFlee::GetInstanceMemorySize() const
{
	return sizeof(FFleeTaskMemory);
}

EBTNodeResult::Type UBTTask_AnimalFlee::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller) return EBTNodeResult::Failed;

	ABaseAnimal* Animal = Cast<ABaseAnimal>(Controller->GetPawn());
	if (!Animal) return EBTNodeResult::Failed;

	AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetActor"));
	if (!TargetActor) return EBTNodeResult::Failed;

	if (ABaseAnimal* TargetAnimal = Cast<ABaseAnimal>(TargetActor))
	{
		if (TargetAnimal->RowName == Animal->RowName)
		{
			Animal->ClearTarget();
			if (UBlackboardComponent* BB = Controller->GetBlackboardComponent())
			{
				BB->ClearValue(TEXT("TargetActor"));
			}
			Animal->SetAnimalState(EAnimalState::Patrol);
			return EBTNodeResult::Failed;
		}
	}

	TargetToFlee = TargetActor;

	FFleeTaskMemory* MyMemory = reinterpret_cast<FFleeTaskMemory*>(NodeMemory);
	MyMemory->ThreatsToFlee.Empty();
	MyMemory->LastThreatUpdateTime = 0.0f;

	UpdateThreats(OwnerComp, NodeMemory);

	return EBTNodeResult::InProgress;
}

void UBTTask_AnimalFlee::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	ABaseAnimal* Animal = Cast<ABaseAnimal>(Controller ? Controller->GetPawn() : nullptr);
	FFleeTaskMemory* MyMemory = reinterpret_cast<FFleeTaskMemory*>(NodeMemory);

	if (!Animal || Animal->CurrentState == EAnimalState::Dead || Animal->CurrentState == EAnimalState::Stun)
	{
		GetWorld()->GetTimerManager().ClearTimer(MyMemory->ThreatUpdateTimer);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (Animal->CurrentState != EAnimalState::Flee)
	{
		GetWorld()->GetTimerManager().ClearTimer(MyMemory->ThreatUpdateTimer);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - MyMemory->LastThreatUpdateTime >= 0.5f)
	{
		UpdateThreats(OwnerComp, NodeMemory);
		MyMemory->LastThreatUpdateTime = CurrentTime;
	}

	if (const FAnimalStatsRow* Stats = Animal->GetStatsRowPtr())
	{
		FVector SelfLocation = Animal->GetActorLocation();
		bool bAllThreatsOutOfRange = true;

		for (AActor* Threat : MyMemory->ThreatsToFlee)
		{
			if (IsValid(Threat))
			{
				float Distance = FVector::Dist(SelfLocation, Threat->GetActorLocation());
				if (Distance <= Stats->TargetLostRange)
				{
					bAllThreatsOutOfRange = false;
					break;
				}
			}
		}

		if (bAllThreatsOutOfRange && MyMemory->ThreatsToFlee.Num() > 0)
		{
			Animal->ClearTarget();
			if (UBlackboardComponent* BB = Controller->GetBlackboardComponent())
			{
				BB->ClearValue(TEXT("TargetActor"));
			}
			Animal->SetAnimalState(EAnimalState::Patrol);
			GetWorld()->GetTimerManager().ClearTimer(MyMemory->ThreatUpdateTimer);
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return;
		}
	}

	if (MyMemory->ThreatsToFlee.Num() > 0)
	{
		FVector SelfLocation = Animal->GetActorLocation();
		FVector ThreatCenter = FVector::ZeroVector;
		int32 ValidThreats = 0;

		for (AActor* Threat : MyMemory->ThreatsToFlee)
		{
			if (IsValid(Threat))
			{
				ThreatCenter += Threat->GetActorLocation();
				ValidThreats++;
			}
		}

		if (ValidThreats > 0)
		{
			ThreatCenter /= ValidThreats;

			const FAnimalStatsRow* Stats = Animal->GetStatsRowPtr();
			float MaxFleeDistance = Stats ? Stats->TargetLostRange : 600.f;

			FVector AwayDirection = (SelfLocation - ThreatCenter);
			AwayDirection.Z = 0.0f;
			AwayDirection = AwayDirection.GetSafeNormal();

			UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
			if (!NavSys) return;

			TArray<float> DistancesToTry = {
				MaxFleeDistance, MaxFleeDistance * 0.75f, MaxFleeDistance * 0.5f, MaxFleeDistance * 0.25f, 200.f
			};

			FNavLocation BestLocation;
			bool bFoundValidLocation = false;

			for (float TestDistance : DistancesToTry)
			{
				FVector TestFleeLocation = SelfLocation + AwayDirection * TestDistance;
				FNavLocation ProjectedLocation;

				if (NavSys->ProjectPointToNavigation(TestFleeLocation, ProjectedLocation, FVector(100.f, 100.f, 300.f)))
				{
					BestLocation = ProjectedLocation;
					bFoundValidLocation = true;
					break;
				}
			}

			if (bFoundValidLocation)
			{
				FAIMoveRequest MoveReq;
				MoveReq.SetGoalLocation(BestLocation.Location);
				MoveReq.SetAcceptanceRadius(Animal->MeleeRange);

				if (AAIController* AIController = Cast<AAIController>(Animal->GetController()))
				{
					AIController->MoveTo(MoveReq);
				}
			}
		}
	}
}

void UBTTask_AnimalFlee::UpdateThreats(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FFleeTaskMemory* MyMemory = reinterpret_cast<FFleeTaskMemory*>(NodeMemory);
	AAIController* Controller = OwnerComp.GetAIOwner();
	ABaseAnimal* Animal = Cast<ABaseAnimal>(Controller ? Controller->GetPawn() : nullptr);
	if (!Animal) return;

	const FAnimalStatsRow* Stats = Animal->GetStatsRowPtr();
	if (!Stats) return;

	MyMemory->ThreatsToFlee.Empty();
	FVector SelfLocation = Animal->GetActorLocation();
	TArray<AActor*> PlayerCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), PlayerCharacters);
	for (AActor* Actor : PlayerCharacters)
	{
		if (IsValid(Actor))
		{
			float Distance = FVector::Dist(SelfLocation, Actor->GetActorLocation());
			if (Distance <= Stats->TargetLostRange)
			{
				MyMemory->ThreatsToFlee.Add(Actor);
			}
		}
	}
	TArray<AActor*> ModularRobots;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AModularRobot::StaticClass(), ModularRobots);
	for (AActor* Actor : ModularRobots)
	{
		if (IsValid(Actor))
		{
			float Distance = FVector::Dist(SelfLocation, Actor->GetActorLocation());
			if (Distance <= Stats->TargetLostRange)
			{
				MyMemory->ThreatsToFlee.Add(Actor);
			}
		}
	}

	TArray<AActor*> Animals;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseAnimal::StaticClass(), Animals);
	for (AActor* Actor : Animals)
	{
		if (ABaseAnimal* OtherAnimal = Cast<ABaseAnimal>(Actor))
		{
			if (IsValid(OtherAnimal) && OtherAnimal != Animal &&
				OtherAnimal->BehaviorTypeEnum == EAnimalBehavior::Aggressive &&
				OtherAnimal->CurrentState != EAnimalState::Dead)
			{
				float Distance = FVector::Dist(SelfLocation, OtherAnimal->GetActorLocation());
				if (Distance <= Stats->TargetLostRange)
				{
					MyMemory->ThreatsToFlee.Add(OtherAnimal);
				}
			}
		}
	}

	TArray<AActor*> Monsters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseMonster::StaticClass(), Monsters);
	for (AActor* Actor : Monsters)
	{
		if (ABaseMonster* Monster = Cast<ABaseMonster>(Actor))
		{
			if (IsValid(Monster) && !Monster->IsDead())
			{
				float Distance = FVector::Dist(SelfLocation, Monster->GetActorLocation());
				if (Distance <= Stats->TargetLostRange)
				{
					MyMemory->ThreatsToFlee.Add(Monster);
				}
			}
		}
	}
	
	TArray<AActor*> AnimalMonsters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAnimalMonster::StaticClass(), AnimalMonsters);
	for (AActor* Actor : AnimalMonsters)
	{
		if (AAnimalMonster* AnimalMonster = Cast<AAnimalMonster>(Actor))
		{
			if (IsValid(AnimalMonster) && AnimalMonster->CurrentState != EAnimalState::Dead)
			{
				float Distance = FVector::Dist(SelfLocation, AnimalMonster->GetActorLocation());
				if (Distance <= Stats->TargetLostRange)
				{
					MyMemory->ThreatsToFlee.Add(AnimalMonster);
				}
			}
		}
	}

	if (AAnimalAIController* AIController = Cast<AAnimalAIController>(Controller))
	{
		AActor* CurrentPlayerTarget = AIController->GetCurrentPlayerTarget();
		if (IsValid(CurrentPlayerTarget) && CurrentPlayerTarget != TargetToFlee)
		{
			TargetToFlee = CurrentPlayerTarget;
			Animal->CurrentTarget = CurrentPlayerTarget;
			if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
			{
				BB->SetValueAsObject(TEXT("TargetActor"), CurrentPlayerTarget);
			}
		}
	}
}

EBTNodeResult::Type UBTTask_AnimalFlee::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FFleeTaskMemory* MyMemory = reinterpret_cast<FFleeTaskMemory*>(NodeMemory);
	GetWorld()->GetTimerManager().ClearTimer(MyMemory->ThreatUpdateTimer);
	return Super::AbortTask(OwnerComp, NodeMemory);
}
