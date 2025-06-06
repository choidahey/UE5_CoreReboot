#include "RegionBossMonster.h"
#include "MonsterAI/Controller/RegionBossMonsterAIController.h"
#include "MonsterAI/Components/MonsterStateComponent.h"
#include "MonsterAI/Region/PatrolRoute.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "NavigationInvokerComponent.h"

ARegionBossMonster::ARegionBossMonster()
	: MyHeader(TEXT("RegionBossMonster"))
{
	PrimaryActorTick.bCanEverTick = false;

	USkeletalMeshComponent* SkeletalMesh = GetMesh();
	SkeletalMesh->SetCollisionObjectType(ECC_Pawn);
	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SkeletalMesh->SetCollisionResponseToAllChannels(ECR_Block);
	SkeletalMesh->SetGenerateOverlapEvents(false);

	NavGenerationRadius = 2000.0f;
	NavRemovalRadius = 2500.0f;

	NavInvoker = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("NavInvoker"));
	NavInvoker->SetGenerationRadii(NavGenerationRadius, NavRemovalRadius);
}

void ARegionBossMonster::BeginPlay()
{
	Super::BeginPlay();

	if (UMonsterStateComponent* StateComp = FindComponentByClass<UMonsterStateComponent>())
	{
		if (!StateComp->IsInState(EMonsterState::Patrol))
		{
			StateComp->SetState(EMonsterState::Patrol);
		}
	}

	SetCombatStartLocation();
}

void ARegionBossMonster::SetCombatStartLocation()
{
	ARegionBossMonsterAIController* AIC = Cast<ARegionBossMonsterAIController>(GetController());
	if (!IsValid(AIC))	return;

	if (auto* BB = AIC->GetBlackboardComponent())
	{
		BB->SetValueAsVector(FRegionBossAIKeys::CombatStartLocation, CombatStartLocation);
	}
}

bool ARegionBossMonster::IsOutsideCombatRange(float Tolerance) const
{
	const FVector CurrentLocation = GetActorLocation();
	const float DistanceSqr = FVector::DistSquared(CombatStartLocation, CurrentLocation);
	const float Limit = FMath::Square(CombatRange + Tolerance);

	return DistanceSqr > Limit;
}

FVector ARegionBossMonster::GetNextPatrolLocation()
{
	if (!PatrolRouteActor) return GetActorLocation();

	int32 PointCount = PatrolRouteActor->GetNumberOfPoints();
	if (PointCount == 0) return GetActorLocation();

	FVector NextPoint = PatrolRouteActor->GetPointAtIndex(CurrentPatrolIndex);
	CurrentPatrolIndex = (CurrentPatrolIndex + 1) % PointCount;

	return NextPoint;
}

