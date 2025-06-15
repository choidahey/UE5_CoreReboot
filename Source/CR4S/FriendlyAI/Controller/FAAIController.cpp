#include "FAAIController.h"
#include "../AnimalFlying.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Algo/RandomShuffle.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

AFAAIController::AFAAIController()
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComp"));
	BlackboardComponent   = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
}

void AFAAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AAnimalFlying* FlyingAnimal = Cast<AAnimalFlying>(InPawn);
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
 
