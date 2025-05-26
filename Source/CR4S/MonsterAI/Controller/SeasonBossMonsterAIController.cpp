#include "MonsterAI/Controller/SeasonBossMonsterAIController.h"
#include "MonsterAI/BossMonster/Season/SeasonBossMonster.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Navigation/PathFollowingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "Character/Characters/PlayerCharacter.h"

ASeasonBossMonsterAIController::ASeasonBossMonsterAIController()
	: MyHeader(TEXT("SeasonBossAIController"))
{
}

void ASeasonBossMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void ASeasonBossMonsterAIController::BeginPlay()
{
	Super::BeginPlay();

	BlackboardComp->SetValueAsVector(FSeasonBossAIKeys::InitializeTargetLocation, GetPlayerInitialLocation());
	BlackboardComp->SetValueAsObject(FSeasonBossAIKeys::NearestHouseActor, GetNearestHouseActor(GetPlayerInitialLocation()));
}

void ASeasonBossMonsterAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject(FAIKeys::TargetActor)))
	{
		SetFocus(Target);
	}
	else
	{
		ClearFocus(EAIFocusPriority::Gameplay);
	}
}

FVector ASeasonBossMonsterAIController::GetPlayerInitialLocation() const
{
	ACharacter* TargetPlayer = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if (!TargetPlayer)
	{
		return FVector::ZeroVector;
	}

	return TargetPlayer->GetActorLocation();
}

AActor* ASeasonBossMonsterAIController::GetNearestHouseActor(const FVector& PlayerLocation) const
{
	TArray<AActor*> Houses;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("House"), Houses);

	if (Houses.Num() <= 0)
	{
		return nullptr;
	}

	float NearestDistance = FLT_MAX;
	AActor* NearestActor = nullptr;

	for (AActor* FindActor : Houses)
	{
		float Distance = FVector::DistSquared(FindActor->GetActorLocation(), PlayerLocation);

		if (Distance < NearestDistance)
		{
			NearestDistance = Distance;
			NearestActor = FindActor;
		}
	}

	return NearestActor;
}
