#include "FishAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "../BaseAnimal.h"
#include "../AnimalFish.h"
#include "Character/Characters/PlayerCharacter.h"

AFishAIController::AFishAIController()
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

void AFishAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    if (AAnimalFish* Fish = Cast<AAnimalFish>(InPawn))
    {
        AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(
            this, &AFishAIController::OnTargetPerceptionUpdated
        );

        if (FishBehaviorTree && BlackboardComponent && BehaviorTreeComponent)
        {
            BlackboardComponent->InitializeBlackboard(*FishBehaviorTree->BlackboardAsset);
            BlackboardComponent->SetValueAsVector(TEXT("LastValidWaterPosition"), Fish->GetActorLocation());
            BlackboardComponent->SetValueAsBool(TEXT("IsInWater"), true);
            SetAnimalState(EAnimalState::Patrol);
            BehaviorTreeComponent->StartTree(*FishBehaviorTree);
        }
    }
}

void AFishAIController::ApplyPerceptionStats(const FAnimalStatsRow& Stats)
{
    CurrentStats = Stats;
    
    SightConfig->SightRadius                  = Stats.SightRadius;
    SightConfig->PeripheralVisionAngleDegrees = Stats.PeripheralVisionAngle;
    HearingConfig->HearingRange               = Stats.HearingRadius;

    AIPerceptionComp->ConfigureSense(*SightConfig);
    AIPerceptionComp->ConfigureSense(*HearingConfig);
    AIPerceptionComp->RequestStimuliListenerUpdate();
}

void AFishAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (ABaseAnimal* Fish = Cast<ABaseAnimal>(GetPawn()))
    {
        if (Stimulus.WasSuccessfullySensed())
        {
            if (ABaseAnimal* SensedAnimal = Cast<ABaseAnimal>(Actor))
            {
                if (SensedAnimal->CurrentState == EAnimalState::Dead)
                {
                    return;
                }

                if (SensedAnimal->RowName == Fish->RowName)
                {
                    return;
                }
            }

            SetTargetActor(Actor);
            HandlePerceptionResponse(Fish);
        }
        else
        {
            if (IsValid(Fish->CurrentTarget))
            {
                float Distance = FVector::Dist(
                    Fish->GetActorLocation(),
                    Fish->CurrentTarget->GetActorLocation()
                );

                if (const FAnimalStatsRow* Stats = Fish->GetStatsRowPtr())
                {
                    if (Distance >= Stats->TargetLostRange)
                    {
                        ClearTargetActor();
                    }
                }
            }
        }
    }
}

void AFishAIController::HandlePerceptionResponse(ABaseAnimal* Fish)
{
    if (!Fish || !Fish->bStatsReady) return;

    switch (Fish->BehaviorTypeEnum)
    {
    case EAnimalBehavior::Aggressive:
        SetAnimalState(EAnimalState::Chase);
        break;

    case EAnimalBehavior::Coward:
        if (ABaseAnimal* TargetAnimal = Cast<ABaseAnimal>(Fish->CurrentTarget))
        {
            if (TargetAnimal->BehaviorTypeEnum == EAnimalBehavior::Aggressive)
            {
                SetAnimalState(EAnimalState::Flee);
            }
        }
        else if (Cast<APlayerCharacter>(Fish->CurrentTarget))
        {
            SetAnimalState(EAnimalState::Flee);
        }
        break;

    case EAnimalBehavior::Passive_AggroOnHit:
        break;
        
    case EAnimalBehavior::Passive_FleeOnHit:
        break;

    default:
        break;
    }
}

void AFishAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    ABaseAnimal* Fish = Cast<ABaseAnimal>(GetPawn());
    if (!Fish || !Fish->bStatsReady || Fish->CurrentState == EAnimalState::Dead)
    {
        return;
    }

    UObject* BBTarget = BlackboardComponent->GetValueAsObject(TEXT("TargetActor"));
    if (!IsValid(BBTarget) && !IsValid(Fish->CurrentTarget))
    {
        ClearTargetActor();
        if (Fish->CurrentState != EAnimalState::Patrol)
        {
            SetAnimalState(EAnimalState::Patrol);
        }
        return;
    }

    if (!IsValid(Fish->CurrentTarget))
    {
        return;
    }

    const FAnimalStatsRow* Stats = Fish->GetStatsRowPtr();
    if (!Stats) return;

    float Distance = FVector::Dist(
        Fish->GetActorLocation(),
        Fish->CurrentTarget->GetActorLocation()
    );

    if (ABaseAnimal* TargetAnimal = Cast<ABaseAnimal>(Fish->CurrentTarget))
    {
        if (TargetAnimal->CurrentState == EAnimalState::Dead)
        {
            ClearTargetActor();
            if (Fish->CurrentState != EAnimalState::Patrol)
            {
                SetAnimalState(EAnimalState::Patrol);
            }
            return;
        }
    }

    if (Fish->BehaviorTypeEnum == EAnimalBehavior::Aggressive ||
        Fish->BehaviorTypeEnum == EAnimalBehavior::Passive_AggroOnHit)
    {
        if (Fish->CurrentState == EAnimalState::Chase || Fish->CurrentState == EAnimalState::Attack)
        {
            if (Distance >= Stats->TargetLostRange)
            {
                ClearTargetActor();
                if (Fish->CurrentState != EAnimalState::Patrol)
                {
                    SetAnimalState(EAnimalState::Patrol);
                }
                return;
            }
        }
    }

    if (Fish->BehaviorTypeEnum == EAnimalBehavior::Coward && Fish->CurrentState == EAnimalState::Flee)
    {
        if (Distance >= Stats->TargetLostRange)
        {
            ClearTargetActor();
            if (Fish->CurrentState != EAnimalState::Patrol)
            {
                SetAnimalState(EAnimalState::Patrol);
            }
            return;
        }
    }
}

void AFishAIController::SetAnimalState(EAnimalState NewState)
{
    if (ABaseAnimal* Fish = Cast<ABaseAnimal>(GetPawn()))
    {
        Fish->SetAnimalState(NewState);
    }
}

void AFishAIController::SetTargetByDamage(AActor* Attacker)
{
    if (!IsValid(Attacker)) return;

    if (ABaseAnimal* Fish = Cast<ABaseAnimal>(GetPawn()))
    {
        const FAnimalStatsRow* Stats = Fish->GetStatsRowPtr();
        if (!Stats) return;

        const float DistanceToNew = FVector::Dist(Fish->GetActorLocation(), Attacker->GetActorLocation());
        const float DistanceToCurrent = IsValid(Fish->CurrentTarget)
            ? FVector::Dist(Fish->GetActorLocation(), Fish->CurrentTarget->GetActorLocation())
            : FLT_MAX;

        const bool bForceTargetSwitch =
            Fish->BehaviorTypeEnum == EAnimalBehavior::Passive_AggroOnHit ||
            Fish->BehaviorTypeEnum == EAnimalBehavior::Passive_FleeOnHit;

        if (bForceTargetSwitch || !IsValid(Fish->CurrentTarget) || DistanceToNew < DistanceToCurrent)
        {
            SetTargetActor(Attacker);

            switch (Fish->BehaviorTypeEnum)
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

void AFishAIController::OnTargetDied()
{
    ClearTargetActor();
    SetAnimalState(EAnimalState::Patrol);
}

void AFishAIController::OnTargetOutOfRange()
{
    SetAnimalState(EAnimalState::Chase);
}

void AFishAIController::OnDied()
{
    SetAnimalState(EAnimalState::Dead);
}

void AFishAIController::SetTargetActor(AActor* Target)
{
    if (ABaseAnimal* Fish = Cast<ABaseAnimal>(GetPawn()))
    {
        if (Fish->CurrentTarget == Target) return;

        Fish->CurrentTarget = Target;
        if (BlackboardComponent)
        {
            BlackboardComponent->SetValueAsObject(TEXT("TargetActor"), Target);
        }
    }
}

void AFishAIController::ClearTargetActor()
{
    if (ABaseAnimal* Fish = Cast<ABaseAnimal>(GetPawn()))
    {
        Fish->CurrentTarget = nullptr;
        if (BlackboardComponent)
        {
            BlackboardComponent->ClearValue(TEXT("TargetActor"));
        }
    }
}