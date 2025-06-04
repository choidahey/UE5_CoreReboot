#pragma once

#include "CoreMinimal.h"
#include "AnimalMovementComponent.h"
#include "GameFramework/Character.h"
#include "FlyingMovementComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UFlyingMovementComponent : public UAnimalMovementComponent
{
    GENERATED_BODY()

public:
    UFlyingMovementComponent();
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    virtual void MoveToLocation(const FVector& Dest) override;
    virtual void MoveToActor(AActor* TargetActor) override;
    virtual void StopMovement() override;
    virtual bool HasReachedDestination(const FVector& Dest, float AcceptanceRadius) const override;

    FTimerHandle& GetFlightTimerHandle();

protected:
    UPROPERTY(EditAnywhere, Category="Flight") float AscendSpeed;
    UPROPERTY(EditAnywhere, Category="Flight") float MinPeak;
    UPROPERTY(EditAnywhere, Category="Flight") float CruiseMinTime;
    UPROPERTY(EditAnywhere, Category="Flight") float TurnAccel;
    UPROPERTY(EditAnywhere, Category="Flight") float MaxYawRate;
    UPROPERTY(EditAnywhere, Category="Flight") float TraceInterval;
    UPROPERTY(EditAnywhere, Category="Flight") float TraceLen;
    UPROPERTY(EditAnywhere, Category="Flight") float SideTraceLen;
    UPROPERTY(EditAnywhere, Category="Flight") int32  MaxSearch;
    UPROPERTY(EditAnywhere, Category="Flight") float LandingTraceLen;
    UPROPERTY(EditAnywhere, Category="Flight") float SideCurveAmp;
    UPROPERTY(EditAnywhere, Category="Flight") float SideCurveHz;
    UPROPERTY(EditAnywhere, Category="Flock") bool  bEnableFlocking;
    UPROPERTY(EditAnywhere, Category="Flock") float FlockRadius;
    UPROPERTY(EditAnywhere, Category="Flock") float AlignWeight;
    UPROPERTY(EditAnywhere, Category="Flock") float CohereWeight;
    UPROPERTY(EditAnywhere, Category="Flock") float SeparateWeight;
    UPROPERTY(EditAnywhere, Category="Flight") float DescendSpeed;

    virtual void InitializeComponent() override;

public:
    enum class EPhase : uint8 { Idle, Ascend, Cruise, Descend };

    FTimerHandle FlightTimer;
    ACharacter*  OwnerCharacter;
    FVector CurrentDir;

    float TargetZ;
    float CruiseElapsed;
    float NextCheckTime;
    int32 SearchCount;
    EPhase Phase;
    
    bool    CanLand(const FVector& CurPos) const;
    float   GroundZAt(const FVector& Pos) const;

    FRandomStream RandomStream;
    FVector TargetLocation;
    bool bHasSeeded = false;

public:
    void SetPhase(EPhase NewPhase) { Phase = NewPhase; }
    EPhase GetPhase() const { return Phase; }

private:
    FVector CalculateJitter(const FVector& Dir) const;
    void InterpolateDirection(const FVector& DesiredDir);
    
    void HandlePhase(float DeltaTime, FVector& Cur);
    void PerformAscend(FVector& Cur, float DeltaTime);
    void CheckAscendComplete(FVector& Cur);
    void PerformCruise(const FVector& Cur, float DeltaTime);
    void UpdateCruiseTimer(float DeltaTime);
    void CheckCruiseComplete();
    void PerformDescend(FVector& Cur, float DeltaTime);
    bool CheckCanLand(const FVector& Cur);
    void ApplyMovement(FVector& Cur, float DeltaTime);
    void ApplyRotation(float DeltaTime);
    void DrawMovementDebug(const FVector& Cur, float DeltaTime);
    FVector ChooseOptimalDirection(const FVector& CurPos) const;

public:
    UPROPERTY(EditAnywhere)
    float FlightSpeed = 600.f;

    UPROPERTY(EditAnywhere)
    float MaxAltitude = 3000.f;

    FVector FlockDir;

public:
    UFUNCTION()
    void SetToFlyingMode();

    UFUNCTION()
    void SetToWalkingMode();

public:
    virtual void BeginPlay() override;
    void StartFlightTick();
    void StopFlightTick();

    void SetTargetLocation(const FVector& NewLocation);
    const FVector& GetTargetLocation() const { return TargetLocation; }    
    
protected:
    UPROPERTY(EditAnywhere, Category="Flight")
    float ForwardSpeed = 800.f;

    UPROPERTY(EditAnywhere, Category="Flight")
    float PitchSpeed = 20.f;

    UPROPERTY(EditAnywhere, Category="Flight")
    float YawSpeed = 40.f;

    UPROPERTY(EditAnywhere, Category="Flight")
    float RollSpeed = 30.f;

    FTimerHandle FlightTickTimer;

private:
    void TickFlight();
    float CurrentPitch = 0.f;
    float CurrentYaw = 0.f;
    float CurrentRoll = 0.f;

protected:
    UFUNCTION()
    void OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

    void HandleCollision();

    UPROPERTY(EditAnywhere)
    bool bUseCollisionToLand = true;

    UPROPERTY(EditAnywhere)
    float FallGravityScale = 1.0f;

    bool bIsFlying = true;

public:
    UFUNCTION()
    void BeginCruiseFromTakeOff();

    UFUNCTION()
    void BeginPerch();

    UFUNCTION()
    void BeginWalkFromPerch();

    UFUNCTION()
    bool ShouldLandNearGroundTarget(float Threshold = 300.0f) const;

    UFUNCTION()
    void UpdateSkyTargetIfNear(float Threshold = 300.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight|Target")
    AActor* CurrentSkyTarget = nullptr;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight|Target")
    TArray<AActor*> SkyTargets;

    AActor* GetCurrentSkyTarget() const { return CurrentSkyTarget; }

    UFUNCTION()
    void UpdateFlightTowardSkyTarget(float DeltaTime);

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MoveSpeed = 600.f;

    FVector DetectObstacleDirection(const FVector& CurDir, bool bDoDownTrace) const;

    UPROPERTY(EditAnywhere, Category="Obstacle Avoidance")
    float ObstacleAvoidanceDownDistance = 1300.0f;
    
    UPROPERTY(EditAnywhere, Category="Obstacle Avoidance")
    float ObstacleAvoidanceStrength = 30.0f;

    float VerticalThrustAmount = 0.0f;

    UFUNCTION()
    void ResetPitchAndRoll();

    UFUNCTION()
    void ChooseRandomGroundTarget();

    UPROPERTY()
    TArray<AActor*> GroundTargets;

    void MoveToGroundTarget(float DeltaTime);
    
    UPROPERTY()
    AActor* GroundTarget = nullptr;
};
