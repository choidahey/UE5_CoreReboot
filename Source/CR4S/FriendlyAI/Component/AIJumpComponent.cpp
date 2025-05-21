#include "AIJumpComponent.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "Navigation/PathFollowingComponent.h"

UAIJumpComponent::UAIJumpComponent()
{
    bAutoActivate = false;
    PrimaryComponentTick.bCanEverTick = false;
    JumpPower = 600.f;
}

void UAIJumpComponent::BeginPlay()
{
    Super::BeginPlay();
    OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (OwnerCharacter)
    {
        OwnerCharacter->LandedDelegate.AddDynamic(this, &UAIJumpComponent::OnCharacterLanded);
    }
}

void UAIJumpComponent::Activate(bool bReset)
{
    Super::Activate(bReset);
    if (GetWorld())
        GetWorld()->GetTimerManager().ClearTimer(EQSTimerHandle);

    StartEQSTimer();
    RunEQSQuery();
}

void UAIJumpComponent::Deactivate()
{
    Super::Deactivate();
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(EQSTimerHandle);
    }
}

void UAIJumpComponent::StartEQSTimer()
{
    if (GetWorld())
    {
        //UE_LOG(LogTemp, Log, TEXT("EQS Timer Start"));
        GetWorld()->GetTimerManager().SetTimer(EQSTimerHandle, this, &UAIJumpComponent::RunEQSQuery, 1.f, true);
    }
}


void UAIJumpComponent::RunEQSQuery()
{
    if (!OwnerCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("RunEQSQuery: OwnerCharacter null"));
        return;
    }
    if (!JumpEQSQuery)
    {
        UE_LOG(LogTemp, Warning, TEXT("RunEQSQuery: JumpEQSQuery null"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("EQS Query Start"));
    if (UEnvQueryInstanceBlueprintWrapper* QueryInstance =
        UEnvQueryManager::RunEQSQuery(this, JumpEQSQuery, OwnerCharacter, EEnvQueryRunMode::SingleResult, nullptr))
    {
        QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &UAIJumpComponent::OnEQSQueryFinished);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("EQS Query Create fail"));
    }
}

void UAIJumpComponent::OnEQSQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
    if (!OwnerCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("OnEQSQueryFinished: OwnerCharacter null"));
        return;
    }
        
    if (!QueryInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("OnEQSQueryFinished: QueryInstance null"));
        return;
    }

    if (!OwnerCharacter->GetCharacterMovement() ||
        OwnerCharacter->GetCharacterMovement()->MovementMode != EMovementMode::MOVE_Walking)
    {
        //UE_LOG(LogTemp, Log, TEXT("Falling, Jump Skip"));
        return;
    }
    
    if (QueryStatus != EEnvQueryStatus::Success)
    {
        UE_LOG(LogTemp, Warning, TEXT("EQS Query fail:  %d"), static_cast<int32>(QueryStatus));
        return;
    }

    const TArray<FVector> Results = QueryInstance->GetResultsAsLocations();
    if (Results.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("EQS Query result null"));
        return;
    }

    const FVector Start = OwnerCharacter->GetActorLocation();
    const FVector End   = Results[0];

    const float Gravity     = FMath::Abs(GetWorld()->GetGravityZ());
    const float InitialVz   = JumpPower;
    const float FlightTime  = (2.f * InitialVz) / Gravity;
    const float DeltaZ      = End.Z - Start.Z;
    const float DeltaXY     = FVector2D(End - Start).Size();
    const float MaxHorDist  = OwnerCharacter->GetCharacterMovement()
        ? OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed * FlightTime
        : 0.f;
    const bool bCanJump     = (DeltaZ <= JumpPower) && (DeltaXY <= MaxHorDist);

    // if (bCanJump)
    // {
    //     UE_LOG(LogTemp, Log, TEXT("Jump : Target (%.1f, %.1f, %.1f)"), End.X, End.Y, End.Z);
    // }
    // else
    // {
    //     UE_LOG(LogTemp, Log, TEXT("Move: Target (%.1f, %.1f, %.1f)"), End.X, End.Y, End.Z);
    // }

    DrawDebugCapsule(GetWorld(), End, 10.f, 5.f, FQuat::Identity,
                     bCanJump ? FColor::Green : FColor::Red, false, 1.f);

    if (bCanJump)
    {
        FVector Dir2D     = (End - Start).GetSafeNormal2D();
        float HorSpeed    = DeltaXY / FlightTime;
        FVector LaunchVel = Dir2D * HorSpeed;
        LaunchVel.Z       = InitialVz;
        OwnerCharacter->LaunchCharacter(LaunchVel, true, true);
    }
    else if (AAIController* AICon = Cast<AAIController>(OwnerCharacter->GetController()))
    {
        FAIMoveRequest MoveReq;
        MoveReq.SetGoalLocation(End);
        MoveReq.SetAcceptanceRadius(5.f);
        MoveReq.SetAllowPartialPath(true);
        AICon->MoveTo(MoveReq);
    }
}

void UAIJumpComponent::OnCharacterLanded(const FHitResult& Hit)
{
    //UE_LOG(LogTemp, Log, TEXT("Landing : Target (%.1f, %.1f, %.1f)"), Hit.Location.X, Hit.Location.Y, Hit.Location.Z);
    RunEQSQuery();
}

void UAIJumpComponent::TryJump()
{
    if (OwnerCharacter && OwnerCharacter->CanJump())
    {
        //UE_LOG(LogTemp, Log, TEXT("TryJump"));
        OwnerCharacter->Jump();
    }
}

void UAIJumpComponent::OnCustomLinkReached(UNavLinkCustomComponent*, UObject*, const FVector&)
{
}
