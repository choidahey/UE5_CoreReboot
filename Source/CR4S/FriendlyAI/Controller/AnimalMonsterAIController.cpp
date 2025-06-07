#include "AnimalMonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "../BaseAnimal.h"
#include "../Component/AIJumpComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "../AnimalMonster.h"
#include "Kismet/GameplayStatics.h"

AAnimalMonsterAIController::AAnimalMonsterAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	BlackboardComponent   = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
}

void AAnimalMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (MonsterBehaviorTree && BlackboardComponent && BehaviorTreeComponent)
	{
		AActor* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (Player)
		{
			BlackboardComponent->InitializeBlackboard(*MonsterBehaviorTree->BlackboardAsset);
			BehaviorTreeComponent->StartTree(*MonsterBehaviorTree);
			BlackboardComponent->SetValueAsObject(TEXT("Player"), Player);
			
			SetTargetActor(Player);
		}
	}
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AAnimalMonsterAIController::SetPlayerToBlackboard, 1.0f, false);
}

void AAnimalMonsterAIController::BeginPlay()
{
	Super::BeginPlay();

	AActor* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (Player && BlackboardComponent)
	{
		BlackboardComponent->InitializeBlackboard(*MonsterBehaviorTree->BlackboardAsset);
		BehaviorTreeComponent->StartTree(*MonsterBehaviorTree);
		SetTargetActor(Player);
		SetAnimalState(EAnimalState::Chase);
		BlackboardComponent->SetValueAsObject(TEXT("Player"), Player);
	}
}

void AAnimalMonsterAIController::SetPlayerToBlackboard()
{
	AActor* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (Player && BlackboardComponent)
	{
		BlackboardComponent->SetValueAsObject(TEXT("Player"), Player);
	}
}

void AAnimalMonsterAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	AAnimalMonster* Monster = Cast<AAnimalMonster>(GetPawn());
	if (!Monster || !Monster->bStatsReady || Monster->CurrentState == EAnimalState::Dead)
	{
		return;
	}
	
	UObject* BBTarget = BlackboardComponent->GetValueAsObject(TEXT("TargetActor"));
	if (!IsValid(BBTarget) || !IsValid(Monster->CurrentTarget))
	{
		ClearTargetActor();
		return;
	}
}

void AAnimalMonsterAIController::SetAnimalState(EAnimalState NewState)
{
	if (ABaseAnimal* Animal = Cast<ABaseAnimal>(GetPawn()))
	{
		Animal->SetAnimalState(NewState);

		if (AAnimalMonster* Monster = Cast<AAnimalMonster>(Animal))
		{
			if (NewState != EAnimalState::Chase && Monster->AIJumpComponent && Monster->AIJumpComponent->IsActive())
			{
				Monster->AIJumpComponent->Deactivate();
			}

			if (NewState == EAnimalState::Attack && Monster->CurrentTarget == nullptr)
			{
				TArray<AActor*> FoundActors;
				UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("House"), FoundActors);
				if (FoundActors.Num() > 0)
				{
					SetTargetActor(FoundActors[0]);
				}
			}
		}
	}
}

void AAnimalMonsterAIController::SetTargetActor(AActor* Target)
{
	if (ABaseAnimal* Animal = Cast<ABaseAnimal>(GetPawn()))
	{
		if (Animal->CurrentTarget == Target) return;

		Animal->CurrentTarget = Target;

		if (BlackboardComponent)
		{
			BlackboardComponent->SetValueAsObject(TEXT("TargetActor"), Target);
		}
	}
}

void AAnimalMonsterAIController::ClearTargetActor()
{
	if (ABaseAnimal* Animal = Cast<ABaseAnimal>(GetPawn()))
	{
		Animal->CurrentTarget = nullptr;

		if (BlackboardComponent)
		{
			BlackboardComponent->ClearValue(TEXT("TargetActor"));
		}
	}
}
