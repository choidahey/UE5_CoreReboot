#include "AnimalAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Touch.h"
#include "../BaseAnimal.h"

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
        Animal->SetAnimalState(EAnimalState::Chase);
        break;

    case EAnimalBehavior::Coward:
        Animal->SetAnimalState(EAnimalState::Flee);
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
    if (!Animal || !Animal->bStatsReady || !IsValid(Animal->CurrentTarget)) return;
    
    const FAnimalStatsRow* Stats = Animal->GetStatsRowPtr();
    if (!Stats) return;

    float Distance = FVector::Dist(
        Animal->GetActorLocation(),
        Animal->CurrentTarget->GetActorLocation()
    );
    if (Animal->BehaviorTypeEnum == EAnimalBehavior::Aggressive && Animal->CurrentState == EAnimalState::Chase)
    {
        if (Distance <= Stats->AttackRange)
        {
            Animal->SetAnimalState(EAnimalState::Attack);
            return;
        }

        if (Distance >= Stats->TargetLostRange)
        {
            Animal->ClearTarget();
            BlackboardComponent->ClearValue(TEXT("TargetActor"));
            Animal->SetAnimalState(EAnimalState::Patrol);
            return;
        }
    }

    if (Animal->BehaviorTypeEnum == EAnimalBehavior::Coward && Animal->CurrentState == EAnimalState::Flee)
    {
        if (Distance >= Stats->TargetLostRange)
        {
            Animal->ClearTarget();
            BlackboardComponent->ClearValue(TEXT("TargetActor"));
            Animal->SetAnimalState(EAnimalState::Patrol);
            return;
        }
    }
    
// #if WITH_EDITOR
//     const FVector Dir = Animal->GetActorForwardVector();
//     float Radius = Stats->SightRadius;
//     float HalfAngleRad = FMath::DegreesToRadians(Stats->PeripheralVisionAngle * 0.5f);
//     DrawDebugCone(GetWorld(), Animal->GetActorLocation(), Dir, Radius, HalfAngleRad, HalfAngleRad, 16, FColor::Green, false, 0.f, 0, 1.f);
//     DrawDebugSphere(GetWorld(), Animal->GetActorLocation(), Stats->HearingRadius, 12, FColor::Yellow, false, 0.f, 0, 1.f);
//     DrawDebugSphere(GetWorld(), Animal->GetActorLocation(), Stats->TargetLostRange, 32, FColor::Orange, false, 0.f, 0, 2.f);
// #endif
}