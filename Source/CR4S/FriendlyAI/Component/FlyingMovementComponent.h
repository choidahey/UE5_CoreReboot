#pragma once

#include "CoreMinimal.h"
#include "AnimalMovementComponent.h"
#include "FlyingMovementComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UFlyingMovementComponent : public UAnimalMovementComponent
{
    GENERATED_BODY()

public:
    UFlyingMovementComponent();
    virtual void BeginPlay() override;
    virtual void MoveToLocation(const FVector& Dest) override;
    virtual void MoveToActor(AActor* TargetActor) override;
    virtual void StopMovement() override;

    FTimerHandle& GetFlightTimerHandle();

protected:
    UPROPERTY(EditAnywhere, Category="Flight") float AscendSpeed;
    UPROPERTY(EditAnywhere, Category="Flight") float ForwardSpeed;
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
    virtual void InitializeComponent() override;
private:
    enum class EPhase : uint8 { Ascend, Cruise, Descend };

    FTimerHandle FlightTimer;
    ACharacter*  OwnerCharacter;
    FVector CurrentDir;

    float TargetZ;
    float CruiseElapsed;
    float NextCheckTime;
    int32 SearchCount;
    EPhase Phase;

    void    TickFlight();
    FVector ChooseDir(const FVector& CurPos) const;
    bool    CanLand(const FVector& CurPos) const;
    float   GroundZAt(const FVector& Pos) const;

    FRandomStream RandomStream;
    FVector TargetLocation;
    bool bHasSeeded = false;
};
