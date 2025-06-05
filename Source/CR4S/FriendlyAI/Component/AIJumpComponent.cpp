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
#include "Navigation/NavLinkProxy.h"
#include "JumpNavLinkComponent.h"
#include "NavLinkCustomComponent.h"
#include "Navigation/GeneratedNavLinksProxy.h"
#include "NavigationSystem.h"
#include "Components/CapsuleComponent.h"
#include "NavMesh/RecastNavMesh.h"

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
        if (UCharacterMovementComponent* MoveComp = OwnerCharacter->GetCharacterMovement())
        {
            CurrentJumpPower = MoveComp->JumpZVelocity;
        }
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
        GetWorld()->GetTimerManager().SetTimer(EQSTimerHandle, this, &UAIJumpComponent::RunEQSQuery, 0.5f, true);
    }
}


void UAIJumpComponent::RunEQSQuery()
{
    AAIController* AICon = Cast<AAIController>(OwnerCharacter->GetController());
    if (!AICon) return;
    UBlackboardComponent* BBComp = AICon->GetBlackboardComponent();
    if (!BBComp) return;

    UEnvQueryInstanceBlueprintWrapper* QueryInstance =
        UEnvQueryManager::RunEQSQuery(this, JumpEQSQuery, OwnerCharacter,
                                      EEnvQueryRunMode::SingleResult, nullptr);
    if (!QueryInstance) return;
    
    QueryInstance->SetNamedParam(FName("MaxJumpHeight"), CurrentJumpPower);

    QueryInstance->GetOnQueryFinishedEvent()
        .AddDynamic(this, &UAIJumpComponent::OnEQSQueryFinished);
}


void UAIJumpComponent::OnEQSQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
    if (!OwnerCharacter)
        return;
    if (QueryStatus != EEnvQueryStatus::Success)
        return;

    const TArray<FVector> Results = QueryInstance->GetResultsAsLocations();
    if (Results.Num() == 0)
        return;

    UCapsuleComponent* Capsule = OwnerCharacter->GetCapsuleComponent();
    FVector RawStart = Capsule->GetComponentLocation() - FVector(0,0,Capsule->GetScaledCapsuleHalfHeight());
    FVector Start;
    if (Results[0].Z > RawStart.Z)
    {
        const float StartOffset = -50.f;
        Start = RawStart - GetOwner()->GetActorForwardVector() * StartOffset;
    }
    else
    {
        Start = RawStart;
    }

    const float EndOffset = 50.f;
    FVector End = Results[0] + GetOwner()->GetActorForwardVector() * EndOffset;

    DrawDebugSphere(GetWorld(), Start, 30.f, 12, FColor::Green, false, 2.f);
    DrawDebugSphere(GetWorld(), End,   30.f, 12, FColor::Red,   false, 2.f);
    DrawDebugLine(GetWorld(), Start, End, FColor::Yellow, false, 2.f, 0, 3.f);

    const float Gravity     = FMath::Abs(GetWorld()->GetGravityZ());
    const float InitialVz   = OwnerCharacter && OwnerCharacter->GetCharacterMovement()
    ? OwnerCharacter->GetCharacterMovement()->JumpZVelocity
    : JumpPower;
    const float FlightTime  = (2.f * InitialVz) / Gravity;
    const float DeltaZ      = End.Z - Start.Z;
    const float DeltaXY     = FVector2D(End - Start).Size();
    const float MaxHorDist  = OwnerCharacter->GetCharacterMovement()
                                 ? OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed * FlightTime
                                 : 0.f;

    const bool bCanJump = (DeltaZ <= JumpPower) && (DeltaXY <= MaxHorDist);
    if (!bCanJump)
    {
        if (AAIController* AICon = Cast<AAIController>(OwnerCharacter->GetController()))
        {
            FAIMoveRequest MoveReq;
            MoveReq.SetGoalLocation(End);
            MoveReq.SetAcceptanceRadius(5.f);
            MoveReq.SetAllowPartialPath(true);
            AICon->MoveTo(MoveReq);
        }
        return;
    }
    
    if (DeltaZ > JumpPower || DeltaXY > MaxHorDist)
    {
        if (AAIController* AICon = Cast<AAIController>(OwnerCharacter->GetController()))
        {
            FAIMoveRequest MoveReq;
            MoveReq.SetGoalLocation(End);
            MoveReq.SetAcceptanceRadius(5.f);
            MoveReq.SetAllowPartialPath(true);
            AICon->MoveTo(MoveReq);
        }
        return;
    }
    
    if (UWorld* World = GetWorld())
    {
        ANavLinkProxy* LinkProxy = CurrentLinkProxy.IsValid() ? CurrentLinkProxy.Get() : nullptr;
        if (!LinkProxy)
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
            LinkProxy = World->SpawnActor<ANavLinkProxy>(Start, FRotator::ZeroRotator, SpawnParams);
            CurrentLinkProxy = LinkProxy;

            if (UNavLinkCustomComponent* Old = LinkProxy->GetSmartLinkComp())
                Old->DestroyComponent();
            UJumpNavLinkComponent* JumpComp = NewObject<UJumpNavLinkComponent>(LinkProxy);
            JumpComp->JumpPower = OwnerCharacter && OwnerCharacter->GetCharacterMovement()
                ? OwnerCharacter->GetCharacterMovement()->JumpZVelocity
                : JumpPower;
            JumpComp->RegisterComponent();
            LinkProxy->AddInstanceComponent(JumpComp);
        }
        else
        {
            LinkProxy->SetActorLocation(Start);
            LinkProxy->PointLinks.Empty();
        }

        FNavigationLink NavLink;
        NavLink.Left      = Start;
        NavLink.Right     = End;
        NavLink.Direction = ENavLinkDirection::BothWays;
        LinkProxy->PointLinks.Add(NavLink);

        if (UJumpNavLinkComponent* JumpComp = LinkProxy->FindComponentByClass<UJumpNavLinkComponent>())
            JumpComp->SetRelativeStartAndEnd(Start, End, LinkProxy);

        if (UNavLinkCustomComponent* SmartComp = LinkProxy->GetSmartLinkComp())
        {
            SmartComp->SetEnabled(true);
        }
        LinkProxy->RegisterAllComponents();
    }
    
    if (AAIController* AICon = Cast<AAIController>(OwnerCharacter->GetController()))
    {
        FAIMoveRequest MoveReq;
        MoveReq.SetGoalLocation(End);
        MoveReq.SetAcceptanceRadius(5.f);
        MoveReq.SetAllowPartialPath(false);
        const FPathFollowingRequestResult MoveResult = AICon->MoveTo(MoveReq);
        if (MoveResult.Code == EPathFollowingRequestResult::RequestSuccessful)
            AICon->ReceiveMoveCompleted.AddDynamic(this, &UAIJumpComponent::OnNavMoveCompleted);
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

void UAIJumpComponent::OnCustomLinkReached(AActor* Agent, const FVector Destination)
{
}

void UAIJumpComponent::OnNavMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
    if (AController* Ctrl = OwnerCharacter ? OwnerCharacter->GetController() : nullptr)
    {
        if (AAIController* AICon = Cast<AAIController>(Ctrl))
        {
            AICon->ReceiveMoveCompleted.RemoveDynamic(this, &UAIJumpComponent::OnNavMoveCompleted);
        }
    }
    if (CurrentLinkProxy.IsValid())
    {
        CurrentLinkProxy->Destroy();
        CurrentLinkProxy = nullptr;
    }
    Deactivate();
}