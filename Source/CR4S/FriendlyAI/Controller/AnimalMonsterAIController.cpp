#include "AnimalMonsterAIController.h"
#include "FriendlyAI/AnimalMonster.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Character/Characters/PlayerCharacter.h"
#include "Character/Characters/ModularRobot.h"
#include "Gimmick/GimmickObjects/Buildings/BaseBuildingGimmick.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "NavFilters/NavigationQueryFilter.h"

#pragma region AAIController Override
AAnimalMonsterAIController::AAnimalMonsterAIController()
{
    PrimaryActorTick.bCanEverTick = true;
    
    BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComp"));
    BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
}

void AAnimalMonsterAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    
    ControlledMonster = Cast<AAnimalMonster>(InPawn);
    if (!IsValid(ControlledMonster))
    {
        UE_LOG(LogTemp, Error, TEXT("AnimalMonsterAIController: Failed to cast pawn to AAnimalMonster"));
        return;
    }

    if (MonsterBehaviorTree && BlackboardComponent && BehaviorTreeComponent)
    {
        BlackboardComponent->InitializeBlackboard(*MonsterBehaviorTree->BlackboardAsset);
        SetMonsterState(EAnimalState::Chase);
        BehaviorTreeComponent->StartTree(*MonsterBehaviorTree);
        
        EvaluateTargetPriority();
    }
}

void AAnimalMonsterAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!IsValid(ControlledMonster)) return;
    
    HandleTargetPriorityLogic();
    UpdateBlackboardValues();
}
#pragma endregion

#pragma region State Management
void AAnimalMonsterAIController::SetMonsterState(EAnimalState NewState)
{
    if (!IsValid(ControlledMonster)) return;

    ControlledMonster->SetAnimalState(NewState);
    
    if (BlackboardComponent)
    {
        BlackboardComponent->SetValueAsEnum(TEXT("AnimalState"), static_cast<uint8>(NewState));
    }
}
#pragma endregion

#pragma region Target Management
void AAnimalMonsterAIController::EvaluateTargetPriority()
{
    if (!IsValid(ControlledMonster)) return;
    
    if (ControlledMonster->bPlayerDead)
    {
        ClearTargetActor();
        SetMonsterState(EAnimalState::Patrol);
        return;
    }
    
    AActor* PlayerTarget = ControlledMonster->GetCurrentPlayerTarget();
        
    AActor* CurrentPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!PlayerTarget && CurrentPawn)
    {
        PlayerTarget = CurrentPawn;
    }
    
    if (IsValid(PlayerTarget) && CanReachTarget(PlayerTarget))
    {
        if (ControlledMonster->PrimaryTarget != PlayerTarget)
        {
            ControlledMonster->SetPrimaryTarget(PlayerTarget);
            SetTargetActor(PlayerTarget);
        }
        SetMonsterState(EAnimalState::Chase);
        return;
    }
    
    if (IsValid(PlayerTarget) && !CanReachTarget(PlayerTarget))
    {
        FindBuildingToDestroy();
        return;
    }
    
    if (IsValid(PlayerTarget))
    {
        if (ControlledMonster->PrimaryTarget != PlayerTarget)
        {
            ControlledMonster->SetPrimaryTarget(PlayerTarget);
            SetTargetActor(PlayerTarget);
        }
        SetMonsterState(EAnimalState::Chase);
    }
}

void AAnimalMonsterAIController::HandlePlayerPathCheck()
{
    if (!IsValid(ControlledMonster)) return;

    AActor* PlayerTarget = ControlledMonster->GetCurrentPlayerTarget();
    bool bCanReachPlayer = IsValid(PlayerTarget) && CanReachTarget(PlayerTarget);
    
    if (bCanReachPlayer != bLastCanReachPlayer)
    {
        bLastCanReachPlayer = bCanReachPlayer;
        
        if (bCanReachPlayer)
        {
            ControlledMonster->SetPrimaryTarget(PlayerTarget);
            ControlledMonster->SetBuildingTarget(nullptr);
            SetTargetActor(PlayerTarget);
            SetMonsterState(EAnimalState::Chase);
        }
    }
}

void AAnimalMonsterAIController::HandleAutomatedAIAttack(AActor* AttackingAI)
{
    if (!IsValid(ControlledMonster) || !IsValid(AttackingAI)) return;
    
    if (CanReachTarget(AttackingAI))
    {
        ControlledMonster->SetSecondaryTarget(AttackingAI);
        
        AActor* PlayerTarget = ControlledMonster->GetCurrentPlayerTarget();
        if (IsValid(PlayerTarget) && CanReachTarget(PlayerTarget))
        {
            return;
        }
        if (!ControlledMonster->bIsAttacking)
        {
            SetTargetActor(AttackingAI);
            SetMonsterState(EAnimalState::Chase);
        }
    }
}

void AAnimalMonsterAIController::SetTargetActor(AActor* Target)
{
    if (!IsValid(ControlledMonster)) return;

    ControlledMonster->CurrentTarget = Target;
    if (BlackboardComponent)
    {
        BlackboardComponent->SetValueAsObject(TEXT("TargetActor"), Target);
    }
}

void AAnimalMonsterAIController::ClearTargetActor()
{
    if (!IsValid(ControlledMonster)) return;

    ControlledMonster->CurrentTarget = nullptr;
    if (BlackboardComponent)
    {
        BlackboardComponent->ClearValue(TEXT("TargetActor"));
    }
}
#pragma endregion

#pragma region Path Management
bool AAnimalMonsterAIController::CheckPathToPlayer()
{
    if (!IsValid(ControlledMonster)) return false;

    AActor* PlayerTarget = ControlledMonster->GetCurrentPlayerTarget();
    return IsValid(PlayerTarget) && CanReachTarget(PlayerTarget);
}

void AAnimalMonsterAIController::FindBuildingToDestroy()
{
    if (!IsValid(ControlledMonster)) return;

    AActor* NearestBuilding = ControlledMonster->FindNearestDestructibleBuilding();
    if (IsValid(NearestBuilding))
    {
        ControlledMonster->SetBuildingTarget(NearestBuilding);
        SetTargetActor(NearestBuilding);
        SetMonsterState(EAnimalState::Attack);
    }
}
#pragma endregion

#pragma region Event Handlers
void AAnimalMonsterAIController::OnBuildingDestroyed()
{
    if (!IsValid(ControlledMonster)) return;
    
    ControlledMonster->SetBuildingTarget(nullptr);
    EvaluateTargetPriority();
}

void AAnimalMonsterAIController::OnTargetReached()
{
    if (!IsValid(ControlledMonster)) return;

    SetMonsterState(EAnimalState::Attack);
}

void AAnimalMonsterAIController::OnPathBlocked()
{
    if (!IsValid(ControlledMonster)) return;
    
    FindBuildingToDestroy();
}

void AAnimalMonsterAIController::OnTargetOutOfRange()
{
    if (!IsValid(ControlledMonster)) return;

    SetMonsterState(EAnimalState::Chase);
}
#pragma endregion

#pragma region Internal Functions
void AAnimalMonsterAIController::UpdateBlackboardValues()
{
    if (!IsValid(ControlledMonster) || !BlackboardComponent) return;
    
    BlackboardComponent->SetValueAsObject(TEXT("PrimaryTarget"), ControlledMonster->PrimaryTarget);
    BlackboardComponent->SetValueAsObject(TEXT("SecondaryTarget"), ControlledMonster->SecondaryTarget);
    BlackboardComponent->SetValueAsObject(TEXT("BuildingTarget"), ControlledMonster->BuildingTarget);
    
    AActor* PlayerTarget = ControlledMonster->GetCurrentPlayerTarget();
    bool bCanReach = IsValid(PlayerTarget) && CanReachTarget(PlayerTarget);
    BlackboardComponent->SetValueAsBool(TEXT("bCanReachPlayer"), bCanReach);
    BlackboardComponent->SetValueAsBool(TEXT("bIsDestroyingBuilding"), ControlledMonster->bIsDestroyingBuilding);

    if (IsValid(PlayerTarget))
    {
        BlackboardComponent->SetValueAsVector(TEXT("LastKnownPlayerLocation"), PlayerTarget->GetActorLocation());
    }
}

void AAnimalMonsterAIController::HandleTargetPriorityLogic()
{
    if (!IsValid(ControlledMonster)) return;
    
    static float LastEvaluationTime = 0.0f;
    float CurrentTime = GetWorld()->GetTimeSeconds();
    
    if (CurrentTime - LastEvaluationTime >= 0.5f)
    {
        LastEvaluationTime = CurrentTime;
        EvaluateTargetPriority();
    }

    HandlePlayerPathCheck();

    CheckDistanceAndUpdateState();
}

bool AAnimalMonsterAIController::CanReachTarget(AActor* Target) const
{
    if (!IsValid(Target) || !IsValid(ControlledMonster)) return false;

    UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
    if (!NavSystem) return false;
    
    const ANavigationData* NavData = NavSystem->GetNavDataForProps(
        ControlledMonster->GetNavAgentPropertiesRef(), 
        ControlledMonster->GetNavAgentLocation()
    );
    
    if (!NavData) 
    {
        return false;
    }
    
    const FSharedConstNavQueryFilter NavFilter = UNavigationQueryFilter::GetQueryFilter(*NavData, this, nullptr);
    
    FPathFindingQuery Query = FPathFindingQuery(
        *ControlledMonster,
        *NavData,
        ControlledMonster->GetNavAgentLocation(),
        Target->GetActorLocation(),
        NavFilter
    );
    
    Query.SetAllowPartialPaths(false);

    FPathFindingResult Result = NavSystem->FindPathSync(Query);
        
    return Result.IsSuccessful() && Result.Path.IsValid();
}

AActor* AAnimalMonsterAIController::FindNearestAttackingAI() const
{
    if (!IsValid(ControlledMonster)) return nullptr;

    TArray<AActor*> AutomatedAI = ControlledMonster->FindNearbyAutomatedAI();
    if (AutomatedAI.Num() == 0) return nullptr;
    
    AActor* NearestAI = nullptr;
    float NearestDistance = FLT_MAX;
    FVector MyLocation = ControlledMonster->GetActorLocation();

    for (AActor* AI : AutomatedAI)
    {
        if (!IsValid(AI)) continue;

        float Distance = FVector::Dist(MyLocation, AI->GetActorLocation());
        if (Distance < NearestDistance)
        {
            NearestDistance = Distance;
            NearestAI = AI;
        }
    }

    return NearestAI;
}

void AAnimalMonsterAIController::SwitchToTarget(AActor* NewTarget)
{
    if (!IsValid(ControlledMonster) || !IsValid(NewTarget)) return;

    SetTargetActor(NewTarget);
    SetMonsterState(EAnimalState::Chase);
}
#pragma endregion

void AAnimalMonsterAIController::CheckDistanceAndUpdateState()
{
    if (!IsValid(ControlledMonster) || !IsValid(ControlledMonster->CurrentTarget) || 
        ControlledMonster->CurrentState == EAnimalState::Dead) return;

    float DistanceToTarget = FVector::Dist(ControlledMonster->GetActorLocation(), 
                                          ControlledMonster->CurrentTarget->GetActorLocation());
    
    bool bCanActuallyAttack = (ControlledMonster->bCanMelee && !ControlledMonster->bIsMeleeOnCooldown && DistanceToTarget <= ControlledMonster->MeleeRange) ||
                         (ControlledMonster->bCanCharge && !ControlledMonster->bIsChargeOnCooldown && DistanceToTarget <= ControlledMonster->DashRange) ||
                         (ControlledMonster->bCanRanged && !ControlledMonster->bIsRangedOnCooldown && DistanceToTarget <= ControlledMonster->RangedRange);

    if (bCanActuallyAttack && ControlledMonster->CurrentState != EAnimalState::Attack)
    {
        SetMonsterState(EAnimalState::Attack);
    }
    else if (!bCanActuallyAttack && ControlledMonster->CurrentState != EAnimalState::Chase)
    {
        SetMonsterState(EAnimalState::Chase);
    }
}

void AAnimalMonsterAIController::OnDied()
{
    SetMonsterState(EAnimalState::Dead);
}