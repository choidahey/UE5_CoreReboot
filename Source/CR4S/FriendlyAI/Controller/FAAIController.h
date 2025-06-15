#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "FAAIController.generated.h"

UCLASS()
class CR4S_API AFAAIController : public AAIController
{
	GENERATED_BODY()

public:
	AFAAIController();

	UPROPERTY()
	TArray<AActor*> FilteredSkyTargets;

	UPROPERTY()
	TArray<AActor*> FilteredGroundTargets;

	UPROPERTY()
	TArray<AActor*> FilteredPerchTargets;
	
protected:
	virtual void OnPossess(APawn* InPawn) override;

#pragma region Components
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|AI")
	TObjectPtr<UBlackboardComponent> BlackboardComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|AI")
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;

#pragma endregion

#pragma region Blackboard Keys
public:
	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector SelfActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blackboard")
	FBlackboardKeySelector RandomPointNearMyLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blackboard")
	FBlackboardKeySelector Enemy;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector DoIFearTheEnemy;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector DistanceToEnemy;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector EnemySpotted;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector ChaseTheEnemy;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector EnemyIsAlive;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector AreWeWalking;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector AreWeFlying;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector AreWePerched;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector Pitch_Adjustment;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector DistanceToTheGround;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector GroundSpeed;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector FlyingSpeed;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector TimeToLand;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector AllPerchesAreFull;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector IsLookingForPerch;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector FlyAway;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector FlyToPerch;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector PlayPerchIdle;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector DownTraceDistance;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector VerticalThrustAmount;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector DoADownTrace;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector StayInSky;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector CanThisBirdPerch;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector CanThisBirdFly;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector GroundSpeedMax;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector FlyingTurnSpeed;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector GroundSpeedMin;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector StoreTargetsInThisRange;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector FlyToPerchSpeed;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector PerchTime;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector FlyTime;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector UsePerchArrow;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector Perch_Offset;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector NextSkyTarget;
#pragma endregion

	void SetDoIFearTheEnemy();
};
