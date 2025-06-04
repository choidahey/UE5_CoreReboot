#include "RegionBossMonster.h"
#include "MonsterAI/Controller/RegionBossMonsterAIController.h"
#include "MonsterAI/Region/PatrolRoute.h"
#include "MonsterAI/Components/MonsterStateComponent.h"
#include "MonsterAI/Components/MonsterAttributeComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
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
		StateComp->SetState(EMonsterState::Patrol);
	}

	SetCombatStartLocation();
}

void ARegionBossMonster::SetCombatStartLocation()
{
	CombatStartLocation = GetActorLocation();

	ARegionBossMonsterAIController* AIC = Cast<ARegionBossMonsterAIController>(GetController());
	if (!IsValid(AIC))	return;

	if (auto* BB = AIC->GetBlackboardComponent())
	{
		BB->SetValueAsVector(FRegionBossAIKeys::CombatStartLocation, CombatStartLocation);
	}
}

bool ARegionBossMonster::IsOutsideCombatRange(float Tolerance) const
{
	const ARegionBossMonsterAIController* AIC = Cast<ARegionBossMonsterAIController>(GetController());
	if (!IsValid(AIC)) return false;

	const UBlackboardComponent* BB = AIC->GetBlackboardComponent();
	if (!IsValid(BB)) return false;

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(FAIKeys::TargetActor));
	if (!IsValid(Target))
	{
		UE_LOG(LogTemp, Warning, TEXT("[RegionBoss] ReturnCheck: Target is invalid"));
		return false;
	}

	const FVector TargetLocation = Target->GetActorLocation();
	const float DistanceSqr = FVector::DistSquared(CombatStartLocation, TargetLocation);
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

void ARegionBossMonster::OnMonsterStateChanged(EMonsterState Previous, EMonsterState Current)	 
{
	Super::OnMonsterStateChanged(Previous, Current);

	const FMonsterAttributeRow& Attribute = AttributeComponent->GetMonsterAttribute();
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (!MoveComp) return;

	float DesiredSpeed = Attribute.MoveSpeed;
	switch (Current)
	{
	case EMonsterState::Patrol: 
		DesiredSpeed *= 0.25f;
		break;
	case EMonsterState::Alert:
		DesiredSpeed *= 0.4f;  
		break;
	case EMonsterState::Combat:
		SetCombatStartLocation();
		break;
	}

	if (!FMath::IsNearlyEqual(MoveComp->MaxWalkSpeed, DesiredSpeed, 1.f))
	{
		MoveComp->MaxWalkSpeed = DesiredSpeed;
	}
}
