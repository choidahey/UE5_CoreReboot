#include "AnimalAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Touch.h"
#include "../BaseAnimal.h"
#include "Components/SphereComponent.h"

AAnimalAIController::AAnimalAIController()
{
    PrimaryActorTick.bCanEverTick = true;
    
    BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComp"));
    BlackboardComponent   = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
    AIPerceptionComp      = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComp"));

    SightConfig   = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));

    SightConfig->DetectionByAffiliation.bDetectEnemies   = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    HearingConfig->DetectionByAffiliation.bDetectEnemies = true;

    AIPerceptionComp->ConfigureSense(*SightConfig);
    AIPerceptionComp->ConfigureSense(*HearingConfig);
}

void AAnimalAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    if (ABaseAnimal* Animal = Cast<ABaseAnimal>(InPawn))
    {
        AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(
            this, &AAnimalAIController::OnTargetPerceptionUpdated
        );
        
        if (BehaviorTreeAsset && BlackboardComponent && BehaviorTreeComponent)
        {
            BlackboardComponent->InitializeBlackboard(
                *BehaviorTreeAsset->BlackboardAsset);
            BlackboardComponent->SetValueAsBool(TEXT("IsTamed"), Animal->bIsTamed);
            SetAnimalState(EAnimalState::Patrol);
            BehaviorTreeComponent->StartTree(*BehaviorTreeAsset);
        }
    }
}


void AAnimalAIController::ApplyPerceptionStats(const FAnimalStatsRow& Stats)
{
    SightConfig->SightRadius                  = Stats.SightRadius;
    SightConfig->PeripheralVisionAngleDegrees = Stats.PeripheralVisionAngle;
    HearingConfig->HearingRange               = Stats.HearingRadius;

    AIPerceptionComp->ConfigureSense(*SightConfig);
    AIPerceptionComp->ConfigureSense(*HearingConfig);
    AIPerceptionComp->RequestStimuliListenerUpdate();
}

void AAnimalAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (ABaseAnimal* Animal = Cast<ABaseAnimal>(GetPawn()))
    {
        if (Stimulus.WasSuccessfullySensed())
        {
            if (ABaseAnimal* SensedAnimal = Cast<ABaseAnimal>(Actor))
            {
                if (SensedAnimal->CurrentState == EAnimalState::Dead ||
                    SensedAnimal->CurrentState == EAnimalState::Stun)
                {
                    return;
                }

                if (SensedAnimal->RowName == Animal->RowName)
                {
                    return;
                }
            }

            SetTargetActor(Actor);
            HandlePerceptionResponse(Animal);
        }
        else
        {
            if (IsValid(Animal->CurrentTarget))
            {
                float Distance = FVector::Dist(
                    Animal->GetActorLocation(),
                    Animal->CurrentTarget->GetActorLocation()
                );

                if (const FAnimalStatsRow* Stats = Animal->GetStatsRowPtr())
                {
                    if (Distance >= Stats->TargetLostRange)
                    {
                        BlackboardComponent->ClearValue(TEXT("TargetActor"));
                        Animal->CurrentTarget = nullptr;
                    }
                }
            }
        }
    }
}

void AAnimalAIController::HandlePerceptionResponse(ABaseAnimal* Animal)
{
    if (!Animal || !Animal->bStatsReady) return;

    switch (Animal->BehaviorTypeEnum)
    {
    case EAnimalBehavior::Aggressive:
        SetAnimalState(EAnimalState::Chase);
        break;

    case EAnimalBehavior::Coward:
        SetAnimalState(EAnimalState::Flee);
        break;

    case EAnimalBehavior::Passive_AggroOnHit:
        break;
        
    case EAnimalBehavior::Passive_FleeOnHit:
        break;

    default:
        break;
    }
}

void AAnimalAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    ABaseAnimal* Animal = Cast<ABaseAnimal>(GetPawn());
    if (!Animal || !Animal->bStatsReady || Animal->CurrentState == EAnimalState::Dead)
    {
        return;
    }

    UObject* BBTarget = BlackboardComponent->GetValueAsObject(TEXT("TargetActor"));
    if (!IsValid(BBTarget) || !IsValid(Animal->CurrentTarget))
    {
        ClearTargetActor();
        if (Animal->CurrentState != EAnimalState::Patrol)
        {
            SetAnimalState(EAnimalState::Patrol);
        }
        return;
    }

    if (!IsValid(BBTarget) && Animal->CurrentState == EAnimalState::Chase)
    {
        //UE_LOG(LogTemp, Warning, TEXT("[Animal] Chase : Blackboard TargetActor null"));
    }

    const FAnimalStatsRow* Stats = Animal->GetStatsRowPtr();
    if (!Stats) return;

    float Distance = FVector::Dist(
        Animal->GetActorLocation(),
        Animal->CurrentTarget->GetActorLocation()
    );

    if (ABaseAnimal* TargetAnimal = Cast<ABaseAnimal>(Animal->CurrentTarget))
    {
        if (TargetAnimal->CurrentState == EAnimalState::Dead)
        {
            ClearTargetActor();
            if (Animal->CurrentState != EAnimalState::Patrol)
            {
                SetAnimalState(EAnimalState::Patrol);
            }
            return;
        }
    }

    if (Animal->BehaviorTypeEnum == EAnimalBehavior::Aggressive ||
    Animal->BehaviorTypeEnum == EAnimalBehavior::Passive_AggroOnHit)
    {
        if (Animal->CurrentState == EAnimalState::Chase)
        {
            // const float AttackDistance = FVector::Dist(Animal->GetActorLocation(), Animal->CurrentTarget->GetActorLocation());
            // if (AttackDistance > 50.f)
            // {
            //     SetAnimalState(EAnimalState::Attack);
            //     return;
            // }

            if (Distance >= Stats->TargetLostRange)
            {
                ClearTargetActor();
                if (Animal->CurrentState != EAnimalState::Patrol)
                {
                    SetAnimalState(EAnimalState::Patrol);
                }
                return;
            }
        }

        if (Animal->CurrentState == EAnimalState::Attack)
        {
            const bool bTargetDead =
                Cast<ABaseAnimal>(Animal->CurrentTarget) &&
                Cast<ABaseAnimal>(Animal->CurrentTarget)->CurrentState == EAnimalState::Dead;

            const bool bOutOfAttackRange =
                !Animal->AttackRange || !Animal->AttackRange->IsOverlappingActor(Animal->CurrentTarget);

            if (!IsValid(Animal->CurrentTarget) || bTargetDead || Distance >= Stats->TargetLostRange)
            {
                ClearTargetActor();

                if (Animal->BehaviorTypeEnum == EAnimalBehavior::Aggressive)
                {
                    SetAnimalState(EAnimalState::Chase);
                }
                else
                {
                    if (Animal->CurrentState != EAnimalState::Patrol)
                    {
                        SetAnimalState(EAnimalState::Patrol);
                    }
                }
                return;
            }

            if (bOutOfAttackRange && Animal->CurrentState != EAnimalState::Chase)
            {
                SetAnimalState(EAnimalState::Chase);
                return;
            }
        }
    }

    if (Animal->BehaviorTypeEnum == EAnimalBehavior::Coward && Animal->CurrentState == EAnimalState::Flee)
    {
        if (Distance >= Stats->TargetLostRange)
        {
            ClearTargetActor();
            if (Animal->CurrentState != EAnimalState::Patrol)
            {
                SetAnimalState(EAnimalState::Patrol);
            }
            return;
        }
    }

    if (Animal && Animal->bStatsReady && Animal->CurrentState != EAnimalState::Dead)
    {
        // const float Radius = Animal->GetCurrentStats().TargetLostRange;
        // const FVector Center = Animal->GetActorLocation();
        //
        // DrawDebugSphere(
        //     GetWorld(),
        //     Center,
        //     Radius,
        //     24,
        //     FColor::Green,
        //     false,
        //     0.1f,
        //     0,
        //     1.5f
        // );
    }
    DrawDebugLine(Animal->GetWorld(),
              Animal->GetActorLocation(),
              Animal->CurrentTarget->GetActorLocation(),
              FColor::Red,
              false,
              0.f,
              0,
              2.f);
}

void AAnimalAIController::SetAnimalState(EAnimalState NewState)
{
    if (ABaseAnimal* Animal = Cast<ABaseAnimal>(GetPawn()))
    {
        Animal->SetAnimalState(NewState);
    }
}

void AAnimalAIController::SetTargetByDamage(AActor* Attacker)
{
    if (!IsValid(Attacker)) return;

    if (ABaseAnimal* Animal = Cast<ABaseAnimal>(GetPawn()))
    {
        const FAnimalStatsRow* Stats = Animal->GetStatsRowPtr();
        if (!Stats) return;

        const float DistanceToNew = FVector::Dist(Animal->GetActorLocation(), Attacker->GetActorLocation());
        const float DistanceToCurrent = IsValid(Animal->CurrentTarget)
            ? FVector::Dist(Animal->GetActorLocation(), Animal->CurrentTarget->GetActorLocation())
            : FLT_MAX;

        const bool bForceTargetSwitch =
            Animal->BehaviorTypeEnum == EAnimalBehavior::Passive_AggroOnHit ||
            Animal->BehaviorTypeEnum == EAnimalBehavior::Passive_FleeOnHit;

        if (bForceTargetSwitch || !IsValid(Animal->CurrentTarget) || DistanceToNew < DistanceToCurrent)
        {
            SetTargetActor(Attacker);

            switch (Animal->BehaviorTypeEnum)
            {
            case EAnimalBehavior::Aggressive:
            case EAnimalBehavior::Passive_AggroOnHit:
                SetAnimalState(EAnimalState::Chase);
                break;

            case EAnimalBehavior::Coward:
            case EAnimalBehavior::Passive_FleeOnHit:
                SetAnimalState(EAnimalState::Flee);
                break;

            default:
                break;
            }
        }
    }
}

void AAnimalAIController::OnTargetDied()
{
    SetTargetByDamage(nullptr);
    SetAnimalState(EAnimalState::Patrol);
}

void AAnimalAIController::OnTargetOutOfRange()
{
    SetAnimalState(EAnimalState::Chase);
}

void AAnimalAIController::OnStunned()
{
    SetAnimalState(EAnimalState::Stun);
}

void AAnimalAIController::OnRecoveredFromStun()
{
    SetAnimalState(EAnimalState::Patrol);
}

void AAnimalAIController::OnDied()
{
    SetAnimalState(EAnimalState::Dead);
}

void AAnimalAIController::SetTargetActor(AActor* Target)
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

void AAnimalAIController::ClearTargetActor()
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
