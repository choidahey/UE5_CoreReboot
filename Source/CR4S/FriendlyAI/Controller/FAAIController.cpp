#include "FAAIController.h"
#include "../AnimalFlying.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Algo/RandomShuffle.h"
#include "Character/Characters/ModularRobot.h"
#include "Character/Characters/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "FriendlyAI/AnimalMonster.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/BaseMonster.h"

AFAAIController::AFAAIController()
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComp"));
	BlackboardComponent   = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));

	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComp"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;

	AIPerceptionComp->ConfigureSense(*SightConfig);
	AIPerceptionComp->ConfigureSense(*HearingConfig);
}

void AFAAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AAnimalFlying* FlyingAnimal = Cast<AAnimalFlying>(InPawn);

	AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(
			this, &AFAAIController::OnTargetPerceptionUpdated
		);
	
	if (FlyingAnimal && FlyingAnimal->BehaviorTreeAsset)
	{
		UBehaviorTree* BTAsset = FlyingAnimal->BehaviorTreeAsset;

		UBlackboardComponent* BBComp = BlackboardComponent.Get();
		if (UseBlackboard(BTAsset->BlackboardAsset, BBComp))
		{
			BlackboardComponent = BBComp;
			RunBehaviorTree(BTAsset);
			SetDoIFearTheEnemy();

			UCharacterMovementComponent* Movement = FlyingAnimal->GetCharacterMovement();
			if (FlyingAnimal->GetBeginLevelFlying())
			{
				if (Movement) Movement->SetMovementMode(MOVE_Flying);
				BlackboardComponent->SetValueAsBool("AreWeWalking", false);
				BlackboardComponent->SetValueAsBool("AreWePerched", false);
				BlackboardComponent->SetValueAsBool("AreWeFlying", true);

				if (FlyingAnimal->GetDisableCollisionDuringFlight())
				{
					//FlyingAnimal->SetActorEnableCollision(false);
				}
				else
				{
					//FlyingAnimal->SetActorEnableCollision(true);
					//FlyingAnimal->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				}
			}
			else if (FlyingAnimal->GetBeginLevelWalking())
			{
				if (Movement) Movement->SetMovementMode(MOVE_Walking);
				BlackboardComponent->SetValueAsBool("AreWeWalking", true);
				BlackboardComponent->SetValueAsBool("AreWePerched", false);
				BlackboardComponent->SetValueAsBool("AreWeFlying", false);
			}
			else if (FlyingAnimal->GetBeginLevelPerched())
			{
				if (Movement) Movement->SetMovementMode(MOVE_Flying);
				BlackboardComponent->SetValueAsBool("AreWeWalking", false);
				BlackboardComponent->SetValueAsBool("AreWePerched", true);
				BlackboardComponent->SetValueAsBool("AreWeFlying", false);
				BlackboardComponent->SetValueAsBool("PlayPerchIdle", true);
			}
		}
	}
}

void AFAAIController::SetDoIFearTheEnemy()
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn || !BlackboardComponent) return;

	AAnimalFlying* FlyingAnimal = Cast<AAnimalFlying>(ControlledPawn);
	if (!FlyingAnimal) return;
	
	bool bDoIFear = FlyingAnimal->GetDoIFearEnemies();
	BlackboardComponent->SetValueAsBool("DoIFearTheEnemy", bDoIFear);
	
	bool bCanPerch = FlyingAnimal->GetCanThisBirdPerch();
	BlackboardComponent->SetValueAsBool("CanThisBirdPerch", bCanPerch);
	
	bool bCanFly = FlyingAnimal->GetCanThisBirdFly();
	BlackboardComponent->SetValueAsBool("CanThisBirdFly", bCanFly);
	
	float FlySpeedMin = FlyingAnimal->GetFlySpeedMin();
	float FlySpeedMax = FlyingAnimal->GetFlySpeedMax();
	float RandomFlySpeed = FMath::FRandRange(FlySpeedMin, FlySpeedMax);
	BlackboardComponent->SetValueAsFloat("FlyingSpeed", RandomFlySpeed);
	
	float GroundMin = FlyingAnimal->GetGroundSpeedMin();
	float GroundMax = FlyingAnimal->GetGroundSpeedMax();
	BlackboardComponent->SetValueAsFloat("GroundSpeedMin", GroundMin);
	BlackboardComponent->SetValueAsFloat("GroundSpeedMax", GroundMax);
	
	float TurnSpeed = FlyingAnimal->GetFlyingTurnSpeed();
	BlackboardComponent->SetValueAsFloat("FlyingTurnSpeed", TurnSpeed);
	
	float Range = FlyingAnimal->GetStoreTargetsInThisRange();
	BlackboardComponent->SetValueAsFloat("StoreTargetsInThisRange", Range);

	float FlyTimeMin = FlyingAnimal->GetFlyTimeMin();
	float FlyTimeMax = FlyingAnimal->GetFlyTimeMax();
	const float RandomFlyTime = FMath::FRandRange(FlyTimeMin, FlyTimeMax);
	BlackboardComponent->SetValueAsFloat(FName("FlyTime"), RandomFlyTime);
	
	float PerchSpeed = FlyingAnimal->GetFlyToPerchSpeed();
	BlackboardComponent->SetValueAsFloat("FlyToPerchSpeed", PerchSpeed);
	
	float PerchMin = FlyingAnimal->GetPerchTimeMin();
	float PerchMax = FlyingAnimal->GetPerchTimeMax();
	float RandomPerchTime = FMath::FRandRange(PerchMin, PerchMax);
	BlackboardComponent->SetValueAsFloat("PerchTime", RandomPerchTime);

	bool bChase = FlyingAnimal->GetChaseTheEnemy();
	BlackboardComponent->SetValueAsBool("ChaseTheEnemy", bChase);

	bool bAllFull = FlyingAnimal->GetAllPerchesAreFull();
	BlackboardComponent->SetValueAsBool("AllPerchesAreFull", bAllFull);

	float Offset = FlyingAnimal->GetPerchOffset();
	BlackboardComponent->SetValueAsFloat("Perch_Offset", Offset);

	bool bUseArrow = FlyingAnimal->GetUsePerchArrow();
	BlackboardComponent->SetValueAsBool("UsePerchArrow", bUseArrow);

	bool bStay = FlyingAnimal->GetStayInSky();
	BlackboardComponent->SetValueAsBool("StayInSky", bStay);

	bool bDoTrace = FlyingAnimal->GetDoADownTrace();
	BlackboardComponent->SetValueAsBool("DoADownTrace", bDoTrace);

	float DownTrace = FlyingAnimal->GetDownTraceDistance();
	BlackboardComponent->SetValueAsFloat("DownTraceDistance", DownTrace);

	float ThrustAmount = FlyingAnimal->GetVerticalThrustAmount();
	BlackboardComponent->SetValueAsFloat("VerticalThrustAmount", ThrustAmount);

	bool bFlyToPerch = FlyingAnimal->GetFlyToPerch();
	BlackboardComponent->SetValueAsBool("FlyToPerch", bFlyToPerch);

	FilteredSkyTargets.Empty();
	TArray<AActor*> FoundActors;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABirdSkyTarget::StaticClass(), FoundActors);
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("SkyTarget"), FoundActors);
	
	const FVector MyLocation = GetPawn()->GetActorLocation();
	const float MaxRange = BlackboardComponent->GetValueAsFloat("StoreTargetsInThisRange");

	for (AActor* Target : FoundActors)
	{
		if (Target && FVector::Dist(MyLocation, Target->GetActorLocation()) <= MaxRange)
		{
			FilteredSkyTargets.Add(Target);
		}
	}
	Algo::RandomShuffle(FilteredSkyTargets);

	if (FilteredSkyTargets.Num() > 0)
	{
		FVector FirstTargetLocation = FilteredSkyTargets[0]->GetActorLocation();
		BlackboardComponent->SetValueAsVector("NextSkyTarget", FirstTargetLocation);
	}
	
	FilteredGroundTargets.Empty();
	TArray<AActor*> FoundGroundTargets;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABirdGroundTarget::StaticClass(), FoundGroundTargets);
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("GroundTarget"), FoundGroundTargets);

	for (AActor* Target : FoundGroundTargets)
	{
		if (Target && FVector::Dist(MyLocation, Target->GetActorLocation()) <= MaxRange)
		{
			FilteredGroundTargets.Add(Target);
		}
	}
	Algo::RandomShuffle(FilteredGroundTargets);

	FilteredPerchTargets.Empty();
	TArray<AActor*> FoundPerchTargets;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("PerchTarget"), FoundPerchTargets);
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABirdPerchTarget::StaticClass(), FoundPerchTargets);

	for (AActor* Target : FoundPerchTargets)
	{
		if (Target && FVector::Dist(MyLocation, Target->GetActorLocation()) <= MaxRange)
		{
			FilteredPerchTargets.Add(Target);
		}
	}

	Algo::RandomShuffle(FilteredPerchTargets);
}
 
void AFAAIController::SetAnimalState(EAnimalState NewState)
{
	if (ABaseAnimal* Animal = Cast<ABaseAnimal>(GetPawn()))
	{
		Animal->SetAnimalState(NewState);
	}
}

void AFAAIController::ApplyPerceptionStats(const FAnimalStatsRow& Stats)
{
	SightConfig->SightRadius = Stats.SightRadius;
	SightConfig->PeripheralVisionAngleDegrees = Stats.PeripheralVisionAngle;
	HearingConfig->HearingRange = Stats.HearingRadius;

	AIPerceptionComp->ConfigureSense(*SightConfig);
	AIPerceptionComp->ConfigureSense(*HearingConfig);
	AIPerceptionComp->RequestStimuliListenerUpdate();
}

void AFAAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (AAnimalFlying* FlyingAnimal = Cast<AAnimalFlying>(GetPawn()))
    {
        if (Stimulus.WasSuccessfullySensed())
        {
            if (FlyingAnimal->BehaviorTypeEnum == EAnimalBehavior::Coward)
            {
                bool bShouldTarget = false;
            	
                if (Cast<APlayerCharacter>(Actor))
                {
                    bShouldTarget = true;
                }
                else if (AModularRobot* SensedRobot = Cast<AModularRobot>(Actor))
                {
                    if (SensedRobot->GetMountedCharacter())
                    {
                        bShouldTarget = true;
                    }
                }
                else if (Cast<ABaseMonster>(Actor))
                {
                    bShouldTarget = true;
                }
                else if (ABaseAnimal* SensedAnimal = Cast<ABaseAnimal>(Actor))
                {
                    if (SensedAnimal->CurrentState != EAnimalState::Dead &&
                        SensedAnimal->CurrentState != EAnimalState::Stun &&
                        SensedAnimal->RowName != FlyingAnimal->RowName &&
                        (SensedAnimal->BehaviorTypeEnum == EAnimalBehavior::Aggressive ||
                         SensedAnimal->BehaviorTypeEnum == EAnimalBehavior::Monster))
                    {
                        bShouldTarget = true;
                    }
                }
                
                if (!bShouldTarget)
                {
                    return;
                }
            }
            else
            {
                if (FlyingAnimal->BehaviorTypeEnum == EAnimalBehavior::Aggressive)
                {
                    if (Cast<AAnimalMonster>(Actor) || Cast<ABaseMonster>(Actor))
                    {
                        return;
                    }
                }

                if (ABaseAnimal* SensedAnimal = Cast<ABaseAnimal>(Actor))
                {
                    if (SensedAnimal->CurrentState == EAnimalState::Dead ||
                        SensedAnimal->CurrentState == EAnimalState::Stun)
                    {
                        return;
                    }

                    if (SensedAnimal->RowName == FlyingAnimal->RowName)
                    {
                        return;
                    }
                }
            }

            if (Cast<APlayerCharacter>(Actor))
            {
                AActor* PlayerTarget = GetCurrentPlayerTarget();
                SetTargetActor(PlayerTarget);
            }
            else if (AModularRobot* SensedRobot = Cast<AModularRobot>(Actor))
            {
                if (SensedRobot->GetMountedCharacter())
                {
                    SetTargetActor(SensedRobot);
                }
            }
            else
            {
                SetTargetActor(Actor);
            }
            
            HandlePerceptionResponse(FlyingAnimal, Actor);
        }
        else
        {
            if (IsValid(FlyingAnimal->CurrentTarget))
            {
                float Distance = FVector::Dist(
                    FlyingAnimal->GetActorLocation(),
                    FlyingAnimal->CurrentTarget->GetActorLocation()
                );

                if (const FAnimalStatsRow* Stats = FlyingAnimal->GetStatsRowPtr())
                {
                    if (Distance >= Stats->TargetLostRange)
                    {
                        BlackboardComponent->ClearValue(TEXT("TargetActor"));
                        FlyingAnimal->CurrentTarget = nullptr;
                    }
                }
            }
        }
    }
}

void AFAAIController::HandlePerceptionResponse(AAnimalFlying* Animal, AActor* SensedActor)
{
	if (!Animal || !Animal->bStatsReady) return;

	switch (Animal->BehaviorTypeEnum)
	{
	case EAnimalBehavior::Aggressive:
		if (BlackboardComponent.Get())
		{
			BlackboardComponent->SetValueAsEnum(TEXT("AnimalState"), static_cast<uint8>(EAnimalState::Chase));
		}
		break;

	case EAnimalBehavior::Coward:
		if (BlackboardComponent.Get())
		{
			BlackboardComponent->SetValueAsBool(TEXT("FlyAway"), true);
			BlackboardComponent->SetValueAsEnum(TEXT("AnimalState"), static_cast<uint8>(EAnimalState::Flee));
		}
		break;
	}
}

void AFAAIController::SetTargetActor(AActor* Target)
{
    if (AAnimalFlying* Animal = Cast<AAnimalFlying>(GetPawn()))
    {
        if (Animal->CurrentTarget == Target) return;

        Animal->CurrentTarget = Target;
        if (BlackboardComponent)
        {
            BlackboardComponent->SetValueAsObject(TEXT("TargetActor"), Target);
        }
    }
}

AActor* AFAAIController::GetCurrentPlayerTarget() const
{
    APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
    if (!IsValid(Player)) return nullptr;
    
    TArray<AActor*> FoundRobots;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AModularRobot::StaticClass(), FoundRobots);

    for (AActor* RobotActor : FoundRobots)
    {
        if (AModularRobot* Robot = Cast<AModularRobot>(RobotActor))
        {
            APlayerCharacter* MountedChar = Robot->GetMountedCharacter();            
            if (MountedChar == Player)
            {
                return Robot;
            }
        }
    }
    return Player;
}