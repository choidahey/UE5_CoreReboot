#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "AIJumpComponent.generated.h"

USTRUCT(BlueprintType)
struct FJumpPoint
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    FVector Location;
    
    UPROPERTY(BlueprintReadWrite, meta = (Bitmask))
    uint8 bRequiresJump : 1;
    
    UPROPERTY(BlueprintReadWrite)
    float RequiredJumpPower;
    
    UPROPERTY(BlueprintReadWrite)
    FVector JumpStartPos;
    
    UPROPERTY(BlueprintReadWrite)
    FVector JumpVelocity;

    FJumpPoint()
        : Location(FVector::ZeroVector)
        , bRequiresJump(0)
        , RequiredJumpPower(0.0f)
        , JumpStartPos(FVector::ZeroVector)
        , JumpVelocity(FVector::ZeroVector)
    {
    }
};

USTRUCT(BlueprintType)
struct FObstacleInfo
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    FVector StartPos;
    
    UPROPERTY(BlueprintReadWrite)
    FVector EndPos;
    
    UPROPERTY(BlueprintReadWrite)
    float Height;
    
    UPROPERTY(BlueprintReadWrite)
    float Width;
    
    UPROPERTY(BlueprintReadWrite, meta = (Bitmask))
    uint8 bIsJumpable : 1;
    
    UPROPERTY(BlueprintReadWrite, meta = (Bitmask))
    uint8 bIsWorldStatic : 1;

    FObstacleInfo()
        : StartPos(FVector::ZeroVector)
        , EndPos(FVector::ZeroVector)
        , Height(0.0f)
        , Width(0.0f)
        , bIsJumpable(0)
        , bIsWorldStatic(1)
    {
    }
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UAIJumpComponent : public UActorComponent
{
    GENERATED_BODY()

#pragma region Core System Management

public:
    UAIJumpComponent();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    
    UFUNCTION(BlueprintCallable, Category = "AI Jump")
    void ActivateJumpComponent(float InTargetReachDistance = 10.0f);
    
    UFUNCTION(BlueprintCallable, Category = "AI Jump")
    void DeactivateJumpComponent();

    UFUNCTION(BlueprintCallable, Category="Jump")
    void UpdateOwnerStats();

    float GetJumpPowerFromOwner();

#pragma endregion

#pragma region Target Tracking System
public:
    bool HasReachedTarget();
    
private:
    bool GetTargetActorFromBlackboard();

#pragma endregion

#pragma region Path Calculation System

private:
    void UpdatePathToTarget();
    bool CalculateOptimalPath(const FVector& StartLocation, const FVector& TargetLocation, TArray<FJumpPoint>& OutPath);
    bool IsDirectPathPossible(const FVector& StartPos, const FVector& EndPos, float JumpPower);
    bool FindNavigationPath(const FVector& StartLocation, const FVector& TargetLocation, TArray<FJumpPoint>& OutPath);
    bool CreateComplexJumpPath(const FVector& StartLocation, const FVector& TargetLocation, TArray<FJumpPoint>& OutPath);

#pragma endregion

#pragma region Movement Execution System

private:
    void ExecuteMovement();
    void ExecuteJump();
    void HandleLanding();
    bool IsGrounded();

#pragma endregion

#pragma region Terrain Analysis System

private:
    bool CanJumpOverObstacle(const FVector& StartPos, const FVector& EndPos, float JumpPower);
    bool IsObstacleJumpable(const FVector& StartPos, const FVector& ObstaclePos, float JumpPower);
    float CalculateObstacleHeight(const FVector& StartPos, const FVector& EndPos);

#pragma endregion

#pragma region Jump Physics Calculation

private:
    FVector CalculateOptimalJumpVelocity(const FVector& StartPos, const FVector& EndPos, float JumpPower, const FVector& WallLocation, float WallHeight);
    bool AnalyzePathObstacles(const FVector& StartPos, const FVector& EndPos, FVector& OutWallLocation, float& OutWallHeight);
    FVector CalculateBasicJump(const FVector& StartPos, const FVector& EndPos, float JumpPower);
    float CalculateSafeStartDistance(float CapsuleRadius, float WallHeight, float JumpPower);
    FVector CalculateOptimalStartPosition(const FVector& StartPos, const FVector& WallLocation, float SafeDistance);
    FVector CalculatePreciseJumpVelocity(const FVector& StartPos, const FVector& EndPos, float JumpPower, float CapsuleRadius, float CapsuleHeight, const FVector& WallLocation, float WallHeight);
    bool ValidateTrajectoryWithCapsule(const FVector& StartPos, const FVector& Velocity, float CapsuleRadius, float CapsuleHeight, const FVector& WallLocation, float WallHeight);
    bool DetectNearbyWall(const FVector& StartPos, FVector& OutWallLocation, float& OutWallHeight);

private:
    void ResetJumpState();

#pragma endregion

#pragma region Member Variables
public:
    UPROPERTY(EditAnywhere)
    uint8 bEnableRotation : 1 = 0;
    
private:
    // UPROPERTY Variables
    UPROPERTY()
    AActor* CurrentTargetActor = nullptr;

    UPROPERTY()
    ACharacter* OwnerCharacter = nullptr;

    UPROPERTY()
    AAIController* AIController = nullptr;

    UPROPERTY()
    UBlackboardComponent* BlackboardComponent = nullptr;

    UPROPERTY()
    UCrowdFollowingComponent* CrowdFollowingComponent = nullptr;

    UPROPERTY()
    TArray<AActor*> CachedStaticActors;

    UPROPERTY()
    TArray<AActor*> CachedDynamicActors;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Jump Settings", meta = (AllowPrivateAccess = "true"))
    float PathRecalculationInterval = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Jump Settings", meta = (AllowPrivateAccess = "true"))
    float TargetReachDistance = 1500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Jump Settings", meta = (AllowPrivateAccess = "true"))
    float JumpStartDistance = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Jump Settings", meta = (AllowPrivateAccess = "true"))
    int32 MaxJumpRetries = 3;

    // UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Jump Settings", meta = (AllowPrivateAccess = "true"))
    // float WallDetectionDistance = 200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug", meta = (AllowPrivateAccess = "true"))
    uint8 bShowDebugInfo : 1 = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug", meta = (AllowPrivateAccess = "true"))
    uint8 bShowDetailedDebug : 1 = 0;

    // Regular Variables
    float LastPathCalculationTime = 0.0f;
    float LastEnvironmentCheckTime = 0.0f;
    float OwnerJumpPower = 0.0f;
    float OwnerMaxSpeed = 0.0f;
    float OwnerJumpDistance = 0.0f;
    int32 CurrentJumpRetryCount = 0;
    int32 CurrentPathIndex = 0;
    int32 CrowdAgentIndex = 0;
    FVector CurrentDestination = FVector::ZeroVector;
    TArray<FJumpPoint> CurrentPath;
    class dtCrowd* DetourCrowd = nullptr;

    // Bool Variables
    uint8 bIsJumping : 1 = 0;
    uint8 bIsComponentActive : 1 = 0;
    uint8 bDetourCrowdInitialized : 1 = 0;
    uint8 bCrowdSystemInitialized : 1 = 0;

#pragma endregion

public:
    bool GetIsComponentActive() const { return bIsComponentActive; }
    
};