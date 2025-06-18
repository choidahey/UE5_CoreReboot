#pragma once

#include "CoreMinimal.h"
#include "BaseAnimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Controller/FAAIController.h"
#include "AnimalFlying.generated.h"

class UFlyingMovementComponent;
class UCapsuleComponent;

UCLASS()
class CR4S_API AAnimalFlying : public ABaseAnimal
{
	GENERATED_BODY()
	
public:
	AAnimalFlying();
	
public:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void OnCapsuleHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	void SetMovementToFlying();
	void SetMovementToWalking();

	UPROPERTY(EditAnywhere, Category="AI")
	UBehaviorTree* BehaviorTreeAsset;
	
#pragma region Stun
public:
	virtual void ApplyStun(float Amount) override;
	virtual void RecoverFromStun() override;
#pragma endregion
	
#pragma region Attack
	//void PerformMeleeAttack() override;
	void PerformChargeAttack() override;
	//void PerformRangedAttack() override;
#pragma endregion

#pragma region Getter/Setter
	bool GetDoIFearEnemies() const { return bDoIFearEnemies; }
	bool GetCanThisBirdPerch() const { return bCanThisBirdPerch; }
	bool GetCanThisBirdFly() const { return bCanThisBirdFly; }
	bool GetBeginLevelWalking() const { return bBeginLevelWalking; }
	bool GetBeginLevelPerched() const { return bBeginLevelPerched; }
	float GetFlySpeedMin() const { return FlySpeedMin; }
	float GetFlySpeedMax() const { return FlySpeedMax; }
	float GetFlyTimeMin() const { return FlyTimeMin; }
	float GetFlyTimeMax() const { return FlyTimeMax; }
	float GetGroundSpeedMin() const { return GroundSpeedMin; }
	float GetGroundSpeedMax() const { return GroundSpeedMax; }
	float GetFlyingTurnSpeed() const { return FlyingTurnSpeed; }
	float GetStoreTargetsInThisRange() const { return StoreTargetsInThisRange; }
	float GetFlyToPerchSpeed() const { return FlyToPerchSpeed; }
	float GetPerchTimeMin() const { return PerchTimeMin; }
	float GetPerchTimeMax() const { return PerchTimeMax; }
	bool GetChaseTheEnemy() const { return bChaseTheEnemy; }
	bool GetDisableCollisionDuringFlight() const { return bDisableCollisionDuringFlight; }
	bool GetAllPerchesAreFull() const { return bAllPerchesAreFull; }
	float GetPerchOffset() const { return PerchOffset; }
	bool GetUsePerchArrow() const { return bUsePerchArrow; }
	bool GetStayInSky() const { return bStayInSky; }
	bool GetDoADownTrace() const { return bDoADownTrace; }
	float GetDownTraceDistance() const { return DownTraceDistance; }
	float GetVerticalThrustAmount() const { return VerticalThrustAmount; }
	bool GetFlyToPerch() const { return bFlyToPerch; }
	bool GetBeginLevelFlying() const { return bBeginLevelFlying; }
	bool GetPlayTakeOffAnimation() const { return bPlayTakeOffAnimation; }
	bool GetAreWeFlying() const { return bAreWeFlying; }
	bool GetAreWeWalking() const { return bAreWeWalking; }
	int32 GetRandomIdleAnimation() const { return IdleAnimIndex; }
	bool GetPlayIdleLoop() const { return bPlayIdleLoop; }
	bool GetFlapFast() const { return bFlapFast; }
	bool GetLandOnPerch() const { return bLandOnPerch; }
	bool GetPlayPerchedIdle() const { return bPlayPerchedIdle; }

	void SetPlayIdleLoop(bool bValue) { bPlayIdleLoop = bValue; }
	void SetFlapFast(bool bValue) { bFlapFast = bValue; }
	void SetLandOnPerch(bool bValue) { bLandOnPerch = bValue; }
	void SetPlayPerchedIdle(bool bValue) { bPlayPerchedIdle = bValue; }

	
#pragma endregion
	
public:
	virtual void Landed(const FHitResult& Hit) override;
	
	UPROPERTY(VisibleAnywhere)
	UFlyingMovementComponent* FlyingComp;

	UPROPERTY()
	UBlackboardComponent* BlackboardComponent;

	void LiftOff();
	void Land();

#pragma region Property
private:
	UPROPERTY(EditAnywhere)
	float AltitudeChangeRate = 0.0f; //?

	UPROPERTY(EditAnywhere)
	float FlightSpeed = 0.0f; //?

	UPROPERTY(EditAnywhere)
	float MaxAltitude = 0.0f; //?

	UPROPERTY(EditAnywhere)
	bool bDoIFearEnemies = true;

	UPROPERTY(EditAnywhere)
	bool bCanThisBirdPerch = true;

	UPROPERTY(EditAnywhere)
	bool bCanThisBirdFly = true;

	UPROPERTY(EditAnywhere)
	bool bChaseTheEnemy = false; //?

	UPROPERTY(EditAnywhere)
	bool bAllPerchesAreFull = false;

	UPROPERTY(EditAnywhere)
	bool bUsePerchArrow = true; //?

	UPROPERTY(EditAnywhere)
	bool bStayInSky = false;

	UPROPERTY(EditAnywhere)
	bool bDoADownTrace = true;

	UPROPERTY(EditAnywhere)
	bool bFlyToPerch = false;

	UPROPERTY(EditAnywhere)
	bool bBeginLevelFlying = true;

	UPROPERTY(EditAnywhere)
	bool bDisableCollisionDuringFlight = true;
	
	UPROPERTY(EditAnywhere)
	bool bBeginLevelWalking  = false;

	UPROPERTY(EditAnywhere)
	bool bBeginLevelPerched = false;
	
	UPROPERTY(EditAnywhere)
	float FlySpeedMin = 600.0f;

	UPROPERTY(EditAnywhere)
	float FlySpeedMax = 800.0f;

	UPROPERTY(EditAnywhere)
	float GroundSpeedMin = 60.0f;

	UPROPERTY(EditAnywhere)
	float GroundSpeedMax = 70.0f;

	UPROPERTY(EditAnywhere)
	float FlyingTurnSpeed = 50.0f;

	UPROPERTY(EditAnywhere)
	float StoreTargetsInThisRange = 500000.0f;

	UPROPERTY(EditAnywhere)
	float FlyTimeMin = 10.0f;

	UPROPERTY(EditAnywhere)
	float FlyTimeMax = 90.0f;
	
	UPROPERTY(EditAnywhere)
	float FlyToPerchSpeed = 0.7f;

	UPROPERTY(EditAnywhere)
	float PerchTimeMin = 10.0f;

	UPROPERTY(EditAnywhere)
	float PerchTimeMax = 120.0f;

	UPROPERTY(EditAnywhere)
	float PerchOffset = 44.0f;

	UPROPERTY(EditAnywhere)
	float DownTraceDistance = 1300.0f;

	UPROPERTY(EditAnywhere)
	float VerticalThrustAmount = 30.0f;

	UPROPERTY(EditAnywhere)
	bool bPlayTakeOffAnimation = false;

	UPROPERTY(EditAnywhere)
	bool bAreWeFlying = false;

	UPROPERTY(EditAnywhere)
	bool bAreWeWalking = false;

	UPROPERTY(EditAnywhere)
	int32 IdleAnimIndex = 0;

	UPROPERTY(EditAnywhere)
	bool bPlayIdleLoop = false;

	UPROPERTY(EditAnywhere)
	bool bFlapFast = false;

	UPROPERTY(EditAnywhere)
	bool bLandOnPerch = false;

	UPROPERTY(EditAnywhere)
	bool bPlayPerchedIdle = false;
	
#pragma endregion
public:
	void UpdateAnimationParametersFromBlackboard();
	FTimerHandle AnimationUpdateTimer;
};
