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
                if (SensedAnimal->CurrentState == EAnimalState::Dead)
                {
                    return;
                }

                if (SensedAnimal->RowName == Animal->RowName)
                {
                    return;
                }
            }
            
            BlackboardComponent->SetValueAsObject(TEXT("TargetActor"), Actor);
            Animal->CurrentTarget = Actor;

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
    if (!Animal || !Animal->bStatsReady)
    {
        //UE_LOG(LogTemp, Warning, TEXT("[Animal] Invalid or not ready animal."));
        return;
    }

    UObject* BBTarget = BlackboardComponent->GetValueAsObject(TEXT("TargetActor"));
    if (!IsValid(Animal->CurrentTarget))
    {
        if (Animal->CurrentState == EAnimalState::Chase)
        {
            //UE_LOG(LogTemp, Warning, TEXT("[Animal] Chase : CurrentTarget null"));
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
            //UE_LOG(LogTemp, Warning, TEXT("[AI] Target is dead. Clear & return to Patrol."));
            Animal->ClearTarget();
            BlackboardComponent->ClearValue(TEXT("TargetActor"));
            SetAnimalState(EAnimalState::Patrol);
            return;
        }
    }

    if (Animal->BehaviorTypeEnum == EAnimalBehavior::Aggressive)
    {
        if (Animal->CurrentState == EAnimalState::Chase)
        {
            if (Animal->AttackRange && Animal->AttackRange->IsOverlappingActor(Animal->CurrentTarget))
            {
                //UE_LOG(LogTemp, Log, TEXT("[Animal] Chase → Attack (Target in range)"));
                SetAnimalState(EAnimalState::Attack);
                return;
            }

            if (Distance >= Stats->TargetLostRange)
            {
                //UE_LOG(LogTemp, Log, TEXT("[Animal] Chase → Patrol (Target too far)"));
                Animal->ClearTarget();
                BlackboardComponent->ClearValue(TEXT("TargetActor"));
                SetAnimalState(EAnimalState::Patrol);
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
                //UE_LOG(LogTemp, Log, TEXT("[Animal] Attack → Patrol (Target lost/dead/out of range)"));
                Animal->ClearTarget();
                BlackboardComponent->ClearValue(TEXT("TargetActor"));
                SetAnimalState(EAnimalState::Patrol);
                return;
            }

            if (bOutOfAttackRange)
            {
                //UE_LOG(LogTemp, Log, TEXT("[Animal] Attack → Chase (Target moved out of range)"));
                SetAnimalState(EAnimalState::Chase);
                return;
            }
        }
    }

    if (Animal->BehaviorTypeEnum == EAnimalBehavior::Coward && Animal->CurrentState == EAnimalState::Flee)
    {
        if (Distance >= Stats->TargetLostRange)
        {
            //UE_LOG(LogTemp, Log, TEXT("[Animal] Flee → Patrol (Target far)"));
            Animal->ClearTarget();
            BlackboardComponent->ClearValue(TEXT("TargetActor"));
            SetAnimalState(EAnimalState::Patrol);
            return;
        }
    }
}

void AAnimalAIController::SetAnimalState(EAnimalState NewState)
{
    if (ABaseAnimal* Animal = Cast<ABaseAnimal>(GetPawn()))
    {
        Animal->SetAnimalState(NewState);
    }
}

