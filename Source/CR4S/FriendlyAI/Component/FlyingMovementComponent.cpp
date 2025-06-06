#include "FlyingMovementComponent.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "Engine/OverlapResult.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

UFlyingMovementComponent::UFlyingMovementComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    AscendSpeed      = 300.f;
    ForwardSpeed     = 300.f;
    MinPeak          = 600.f;
    CruiseMinTime    = 3.f;
    TurnAccel        = 4.f;
    MaxYawRate       = 180.f;
    TraceInterval    = 0.02f;
    TraceLen         = 800.f;
    SideTraceLen     = 1200.f;
    MaxSearch        = 5;
    LandingTraceLen  = 4000.f;
    SideCurveAmp     = 50.f;
    SideCurveHz      = 1.f;
    DescendSpeed     = 300.f;
    bEnableFlocking  = true;
    FlockRadius      = 200.f;
    AlignWeight      = 1.f;
    CohereWeight     = 1.f;
    SeparateWeight   = 2.f;

    CurrentDir = FVector::ForwardVector;
}

void UFlyingMovementComponent::BeginPlay()
{
    Super::BeginPlay();
    StartFlightTick();
    if (GetOwner()) GetOwner()->OnActorHit.AddDynamic(this, &UFlyingMovementComponent::OnHit);

    TArray<AActor*> OutActors;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("SkyTarget"), OutActors);
    for (AActor* Actor : OutActors)
    {
        if (Actor) SkyTargets.Add(Actor);
    }
}

void UFlyingMovementComponent::StartFlightTick()
{
    GetWorld()->GetTimerManager().SetTimer(
        FlightTickTimer,
        this,
        &UFlyingMovementComponent::TickFlight,
        0.01f,
        true
    );
}

void UFlyingMovementComponent::StopFlightTick()
{
    GetWorld()->GetTimerManager().ClearTimer(FlightTickTimer);
}

void UFlyingMovementComponent::TickFlight()
{
    //UE_LOG(LogTemp, Warning, TEXT("[TickFlight] Called"));
    AActor* Owner = GetOwner();
    if (!Owner) return;

    const float DeltaTime = GetWorld()->GetDeltaSeconds();

    FVector MyLocation = Owner->GetActorLocation();
    FVector ToTarget = (TargetLocation - MyLocation).GetSafeNormal();

    CurrentDir = DetectObstacleDirection(ToTarget, /*bDoDownTrace=*/ true);

    FVector LocalDir = Owner->GetTransform().InverseTransformVector(CurrentDir);
    
    LocalDir.Normalize();
    
    float TargetPitchInput = -LocalDir.Z;
    CurrentPitch = FMath::FInterpTo(CurrentPitch, TargetPitchInput, DeltaTime, 2.f);

    float TargetYawInput = LocalDir.Y;
    CurrentYaw = FMath::FInterpTo(CurrentYaw, TargetYawInput, DeltaTime, 2.f);
    
    CurrentRoll = FMath::FInterpTo(CurrentRoll, TargetYawInput * 0.5f, DeltaTime, 2.f);

    FRotator DeltaRot = FRotator(
        PitchSpeed * DeltaTime * CurrentPitch,
        YawSpeed * DeltaTime * CurrentYaw,
        RollSpeed * DeltaTime * CurrentRoll
    );
    Owner->AddActorLocalRotation(DeltaRot);

    FVector ForwardMove = CurrentDir * ForwardSpeed * DeltaTime;
    Owner->AddActorLocalOffset(ForwardMove, true);

    const float DistanceSq = FVector::DistSquared(MyLocation, TargetLocation);
    const float ThresholdSq = FMath::Square(300.f);
    if (DistanceSq < ThresholdSq)
    {
        FVector Forward = Owner->GetActorForwardVector();
        FVector NewTarget = MyLocation + Forward * 1000.f;
        SetTargetLocation(NewTarget);
    }
    // UE_LOG(LogTemp, Warning, TEXT("[TickFlight] Current Location: %s → Target: %s"), *GetOwner()->GetActorLocation().ToString(), *TargetLocation.ToString());
    //
    // UE_LOG(LogTemp, Warning, TEXT("[TickFlight] Phase: %d | CurrentDir: %s | Target: %s"), 
    // static_cast<uint8>(Phase),
    // *CurrentDir.ToString(),
    // *TargetLocation.ToString());
}

void UFlyingMovementComponent::InitializeComponent()
{
    Super::InitializeComponent();
    RandomStream.Initialize((int32)(PTRINT(this)));
}

void UFlyingMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bIsFlying && GroundTarget)
    {
        const FVector CurrentLocation = GetOwner()->GetActorLocation();
        const float DistanceToTarget = FVector::Dist(CurrentLocation, GroundTarget->GetActorLocation());

        if (DistanceToTarget < 2000.0f)
        {
            bIsFlying = false;
            
            ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());
            if (CharacterOwner)
            {
                UCharacterMovementComponent* CharMove = CharacterOwner->GetCharacterMovement();
                if (CharMove)
                {
                    CharMove->SetMovementMode(EMovementMode::MOVE_Walking);
                    CharMove->GravityScale = 0.6f;
                    
                    const FVector ForwardImpulse = CharacterOwner->GetActorForwardVector() * 700.0f;
                    const FVector Up = CharacterOwner->GetActorUpVector() * 400.0f;
                    CharMove->Velocity = ForwardImpulse + Up;
                    
                    UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
                    if (Capsule)
                    {
                        Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
                    }
                }
            }
        }
    }
}

FTimerHandle& UFlyingMovementComponent::GetFlightTimerHandle()
{
    return FlightTimer;
}

void UFlyingMovementComponent::MoveToLocation(const FVector& Dest)
{
    if (!bHasSeeded)
    {
        RandomStream.Initialize((int32)(PTRINT(this)));
        bHasSeeded = true;
    }

    const float JitterRadius = 200.f;
    FVector ModifiedDest = Dest + RandomStream.VRand() * RandomStream.FRandRange(0.f, JitterRadius);

    OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (!OwnerCharacter)
    {
        return;
    }

    TargetLocation = ModifiedDest;
    TargetZ        = FMath::Max(TargetLocation.Z, OwnerCharacter->GetActorLocation().Z + MinPeak);
    CruiseElapsed  = 0.f;
    SearchCount    = 0;
    Phase          = EPhase::Ascend;
    CurrentDir     = (TargetLocation - OwnerCharacter->GetActorLocation()).GetSafeNormal();

    auto* Move = OwnerCharacter->GetCharacterMovement();
    Move->SetMovementMode(MOVE_Flying);
    OwnerCharacter->bUseControllerRotationYaw = false;
    Move->bOrientRotationToMovement         = false;

    GetWorld()->GetTimerManager().SetTimer(
        FlightTimer, this, &UFlyingMovementComponent::TickFlight,
        TraceInterval, true, 0.f);

    //UE_LOG(LogTemp, Warning, TEXT("[MoveToLocation] Moving to %s"), *TargetLocation.ToString());

    if (TargetLocation != FVector::ZeroVector)
    {
        DrawDebugLine(GetWorld(), GetOwner()->GetActorLocation(), TargetLocation, FColor::Green, false, -1.f, 0, 2.f);
    }
}

void UFlyingMovementComponent::MoveToActor(AActor* TargetActor)
{
    if (TargetActor)
    {
        MoveToLocation(TargetActor->GetActorLocation());
    }
}

void UFlyingMovementComponent::StopMovement()
{
    GetWorld()->GetTimerManager().ClearTimer(FlightTimer);
    if (OwnerCharacter)
    {
        OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
    }
}



// void UFlyingMovementComponent::UpdateDirection(const FVector& Cur)
// {
//     FVector BaseDir   = DetectObstacleDirection();
//     FVector Jittered  = CalculateJitter(BaseDir);
//     InterpolateDirection(Jittered);
// }

FVector UFlyingMovementComponent::CalculateJitter(const FVector& Dir) const
{
    const float JitterStrength = 1.f;
    return Dir + RandomStream.VRand() * JitterStrength;
}

void UFlyingMovementComponent::InterpolateDirection(const FVector& DesiredDir)
{
    CurrentDir = FMath::VInterpTo(CurrentDir, DesiredDir.GetSafeNormal(), TraceInterval, TurnAccel).GetSafeNormal();
}

void UFlyingMovementComponent::HandlePhase(float DeltaTime, FVector& Cur)
{
    switch (Phase)
    {
    case EPhase::Idle:
        break;

    case EPhase::Ascend:
        PerformAscend(Cur, DeltaTime);
        CheckAscendComplete(Cur);
        break;

    case EPhase::Cruise:
        PerformCruise(Cur, DeltaTime);
        UpdateCruiseTimer(DeltaTime);
        CheckCruiseComplete();
        break;

    case EPhase::Descend:
        PerformDescend(Cur, DeltaTime);
        break;
    }
}

void UFlyingMovementComponent::PerformAscend(FVector& Cur, float DeltaTime)
{
    Cur.Z += AscendSpeed * DeltaTime;
    DrawDebugLine(GetWorld(),
                  FVector(Cur.X, Cur.Y, Cur.Z - AscendSpeed * DeltaTime),
                  Cur, FColor::Yellow, false, TraceInterval, 0, 1.f);
}

void UFlyingMovementComponent::CheckAscendComplete(FVector& Cur)
{
    if (Cur.Z >= TargetZ)
    {
        Cur.Z = TargetZ;
        Phase = EPhase::Cruise;
        CruiseElapsed = 0.f;
    }
}

void UFlyingMovementComponent::PerformCruise(const FVector& Cur, float DeltaTime)
{
    DrawDebugCircle(GetWorld(), Cur,
                    200.f, 16, FColor::Cyan,
                    false, TraceInterval, 0, 1.f,
                    FVector::UpVector, FVector::RightVector, false);
    if (bEnableFlocking)
    {
        FVector Separation = FVector::ZeroVector;
        FVector Alignment  = FVector::ZeroVector;
        FVector Cohesion   = FVector::ZeroVector;
        int32  NeighborCount = 0;
        
        TArray<FOverlapResult> Overlaps;
        FCollisionShape Sphere = FCollisionShape::MakeSphere(FlockRadius);
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(GetOwner());
        if (GetWorld()->OverlapMultiByChannel(Overlaps, Cur, FQuat::Identity, ECC_Pawn, Sphere, Params))
        {
            for (auto& Result : Overlaps)
            {
                AActor* Other = Result.GetActor();
                if (Other && Other != GetOwner())
                {
                    FVector ToOther = Other->GetActorLocation() - Cur;
                    float Dist      = ToOther.Size();
                    if (Dist > 0.f && Dist < FlockRadius)
                    {
                        NeighborCount++;
                        Separation += (Cur - Other->GetActorLocation()) / Dist;
                        if (UFlyingMovementComponent* OtherComp = Cast<UFlyingMovementComponent>(Other->FindComponentByClass<UFlyingMovementComponent>()))
                        {
                            Alignment += OtherComp->CurrentDir;
                        }
                        Cohesion += Other->GetActorLocation();
                    }
                }
            }
        }

        if (NeighborCount > 0)
        {
            Alignment  = (Alignment / NeighborCount).GetSafeNormal() * AlignWeight;
            Cohesion   = ((Cohesion / NeighborCount) - Cur).GetSafeNormal() * CohereWeight;
            Separation = Separation.GetSafeNormal() * SeparateWeight;
            
            FVector FlockVec = (Alignment + Cohesion + Separation).GetSafeNormal();
            FlockDir = FlockVec;
        }
        else
        {
            FlockDir = Cur;
        }
        
        CurrentDir = FMath::VInterpTo(CurrentDir, FlockDir, DeltaTime, TurnAccel).GetSafeNormal();
    }
}

void UFlyingMovementComponent::UpdateCruiseTimer(float DeltaTime)
{
    CruiseElapsed += DeltaTime;
}

void UFlyingMovementComponent::CheckCruiseComplete()
{
    if (CruiseElapsed >= CruiseMinTime)
    {
        Phase = EPhase::Descend;
    }
}

void UFlyingMovementComponent::PerformDescend(FVector& Cur, float DeltaTime)
{
    if (CheckCanLand(Cur))
    {
        StopMovement();
        Phase = EPhase::Idle;
        return;
    }

    float GroundZ = GroundZAt(Cur + CurrentDir);     
    Cur.Z = FMath::FInterpTo(Cur.Z, GroundZ, DeltaTime, DescendSpeed);

    DrawDebugLine(GetWorld(),
                  FVector(Cur.X, Cur.Y, Cur.Z + DescendSpeed * DeltaTime),
                  Cur, FColor::Red, false, TraceInterval, 0, 1.f);
}

bool UFlyingMovementComponent::CheckCanLand(const FVector& Cur)
{
    float Z = GroundZAt(Cur + CurrentDir); 
    if (Z < 0.f)
    {
        return false;
    }

    bool bCanLand = Cur.Z - Z < 150.f;
    DrawDebugCircle(GetWorld(), FVector(Cur.X, Cur.Y, Z),
                    40.f, 12, bCanLand ? FColor::Green : FColor::Red,
                    false, TraceInterval, 0, 2.f,
                    FVector::YAxisVector, FVector::ZAxisVector, false);

    // UE_LOG(LogTemp, Warning, TEXT("[CheckCanLand] CurZ: %.2f, GroundZ: %.2f, CanLand: %s"), 
    // Cur.Z, Z, bCanLand ? TEXT("Yes") : TEXT("No"));
    
    return bCanLand;
}

void UFlyingMovementComponent::ApplyMovement(FVector& Cur, float DeltaTime)
{
    Cur += CurrentDir * ForwardSpeed * DeltaTime;
    OwnerCharacter->SetActorLocation(Cur, true);

    //UE_LOG(LogTemp, Warning, TEXT("[ApplyMovement] Moved to: %s"), *Cur.ToString());
}

void UFlyingMovementComponent::ApplyRotation(float DeltaTime)
{
    FRotator TargetRot = CurrentDir.Rotation();
    FRotator NewRot = FMath::RInterpConstantTo(
        OwnerCharacter->GetActorRotation(), TargetRot,
        DeltaTime, MaxYawRate);
    if (AController* C = OwnerCharacter->GetController())
    {
        C->SetControlRotation(NewRot);
    }
    OwnerCharacter->SetActorRotation(NewRot);
}

void UFlyingMovementComponent::DrawMovementDebug(const FVector& Cur, float DeltaTime)
{
    FVector ForwardEnd = Cur + CurrentDir * TraceLen;
    DrawDebugLine(GetWorld(), Cur, ForwardEnd, FColor::Yellow, false, TraceInterval);
    
    FVector RightVec = FVector::CrossProduct(FVector::UpVector, CurrentDir).GetSafeNormal();
    DrawDebugLine(GetWorld(), Cur, Cur + RightVec * SideTraceLen, FColor::Blue, false, TraceInterval);
    DrawDebugLine(GetWorld(), Cur, Cur - RightVec * SideTraceLen, FColor::Blue, false, TraceInterval);
    
    DrawDebugDirectionalArrow(GetWorld(), Cur, Cur + CurrentDir * 200.f, 50.f, FColor::Green, false, TraceInterval, 0, 2.f);
    DrawDebugSphere(GetWorld(), FVector(Cur.X, Cur.Y, TargetZ), 30.f, 12, FColor::Cyan, false, TraceInterval);
}

FVector UFlyingMovementComponent::ChooseOptimalDirection(const FVector& CurPos) const
{
    FVector PrevDir = CurrentDir;
    FVector DesiredDir = PrevDir;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());
    
    FVector ForwardStart = CurPos;
    FVector ForwardEnd   = CurPos + PrevDir * TraceLen;
    FHitResult HitForward;
    if (GetWorld()->LineTraceSingleByChannel(HitForward, ForwardStart, ForwardEnd, ECC_WorldStatic, Params))
    {
        float YawOffset = (RandomStream.FRand() < 0.5f ? -15.f : 15.f);
        DesiredDir = (PrevDir.RotateAngleAxis(YawOffset, FVector::UpVector)).GetSafeNormal();
    }
    
    FVector RightVec = FVector::CrossProduct(FVector::UpVector, PrevDir).GetSafeNormal();
    for (int32 Side : { -1, 1 })
    {
        FVector SideStart = CurPos;
        FVector SideEnd   = CurPos + RightVec * Side * SideTraceLen;
        FHitResult HitSide;
        if (GetWorld()->LineTraceSingleByChannel(HitSide, SideStart, SideEnd, ECC_WorldStatic, Params))
        {
            float YawOffset = Side * -15.f;
            DesiredDir = (PrevDir.RotateAngleAxis(YawOffset, FVector::UpVector)).GetSafeNormal();
            break;
        }
    }
    
    FVector Fwd  = DesiredDir;
    FVector L45  = Fwd.RotateAngleAxis(-45.f, FVector::UpVector);
    FVector R45  = Fwd.RotateAngleAxis( 45.f, FVector::UpVector);
    FVector Up   = FVector::UpVector;
    FVector Down = -Up;
    const FVector Cands[5] = { Fwd, L45, R45, Up, Down };

    TArray<FVector> ValidDirs;
    const float CheckDist = 500.f;
    const float DownLen   = 1000.f;

    for (const FVector& Dir : Cands)
    {
        FVector TestPt     = CurPos + Dir * CheckDist;
        FVector TraceStart = TestPt + FVector(0, 0, 50.f);
        FVector TraceEnd   = TestPt - FVector(0, 0, DownLen);

        FHitResult FloorHit;
        if (!GetWorld()->LineTraceSingleByChannel(FloorHit, TraceStart, TraceEnd, ECC_Visibility))
        {
            continue;
        }
        
        FVector Start2 = CurPos + Dir * 50.f;
        float Len = (Dir.Equals(L45, 0.01f) || Dir.Equals(R45, 0.01f))
            ? SideTraceLen : TraceLen;
        FVector End2 = Start2 + Dir * Len;

        FHitResult Hit2;
        bool bHit2 = GetWorld()->LineTraceSingleByChannel(Hit2, Start2, End2, ECC_Visibility);
        
        DrawDebugLine(
            GetWorld(),
            Start2, End2,
            bHit2 ? FColor::Red : FColor::Green,
            false,
            TraceInterval,
            0,
            2.f
        );

        if (!bHit2)
        {
            ValidDirs.Add(Dir);
        }
    }

    if (ValidDirs.Num() > 0)
    {
        DesiredDir = ValidDirs[RandomStream.RandRange(0, ValidDirs.Num() - 1)].GetSafeNormal();
    }
    
    return FMath::VInterpTo(PrevDir, DesiredDir, TraceInterval, TurnAccel).GetSafeNormal();
}

float UFlyingMovementComponent::GroundZAt(const FVector& Pos) const
{
    FVector End = Pos - FVector(0, 0, LandingTraceLen);
    FHitResult Hit;
    bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Pos, End, ECC_Visibility);
    
    DrawDebugLine(
        GetWorld(),
        Pos, End,
        bHit ? FColor::Green : FColor::Red,
        false,
        0.f,
        0,
        2.f
    );

    return bHit ? Hit.Location.Z : -1.f;
}

bool UFlyingMovementComponent::CanLand(const FVector& CurPos) const
{
    float Z = GroundZAt(CurPos);
    if (Z < 0.f)
    {
        return false;
    }

    bool bCanLand = CurPos.Z - Z < 150.f;
    DrawDebugCircle(
        GetWorld(),
        FVector(CurPos.X, CurPos.Y, Z),
        40.f, 12,
        bCanLand ? FColor::Green : FColor::Red,
        false,
        TraceInterval,
        0,
        2.f,
        FVector::YAxisVector,
        FVector::ZAxisVector,
        false
    );
    return bCanLand;
}

void UFlyingMovementComponent::SetToFlyingMode()
{
    if (!OwnerCharacter)
    {
        OwnerCharacter = Cast<ACharacter>(GetOwner());
    }
    if (OwnerCharacter)
    {
        OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
        SetPhase(EPhase::Ascend);
    }
    //UE_LOG(LogTemp, Warning, TEXT("[SetToFlyingMode] Called."));
}

void UFlyingMovementComponent::SetToWalkingMode()
{
    if (!OwnerCharacter)
    {
        OwnerCharacter = Cast<ACharacter>(GetOwner());
    }
    if (OwnerCharacter)
    {
        OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
        SetPhase(EPhase::Idle);
    }
}

void UFlyingMovementComponent::SetTargetLocation(const FVector& NewLocation)
{
    TargetLocation = NewLocation;
    //UE_LOG(LogTemp, Warning, TEXT("[SetTargetLocation] Target = %s, Location = %s"), *GetOwner()->GetName(), *TargetLocation.ToString());
}

void UFlyingMovementComponent::HandleCollision()
{
    // TODO : 
}

void UFlyingMovementComponent::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
    if (!bUseCollisionToLand) return;

    if (Hit.bBlockingHit)
    {
        if (ACharacter* Character = Cast<ACharacter>(SelfActor))
        {
            if (UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement())
            {
                MoveComp->SetMovementMode(MOVE_Walking);
                MoveComp->GravityScale = FallGravityScale;
            }
        }
    }
}

bool UFlyingMovementComponent::HasReachedDestination(const FVector& Dest, float AcceptanceRadius) const
{
    if (OwnerCharacter)
    {
        const float Distance = FVector::Dist(OwnerCharacter->GetActorLocation(), Dest);
        return Distance <= AcceptanceRadius;
    }
    return false;
}

// BirdSetMovementTOFlying
void UFlyingMovementComponent::BeginCruiseFromTakeOff()
{
    if (!OwnerCharacter)
    {
        OwnerCharacter = Cast<ACharacter>(GetOwner());
    }

    if (OwnerCharacter)
    {
        UCharacterMovementComponent* Move = OwnerCharacter->GetCharacterMovement();
        if (Move)
        {
            Move->SetMovementMode(MOVE_Flying);
        }

        SetPhase(EPhase::Ascend);
        bIsFlying = true;

        StartFlightTick();
    }

    //UE_LOG(LogTemp, Warning, TEXT("[BeginCruiseFromTakeOff] Entered Ascend Phase"));
}

// SetMovementTOPerched
void UFlyingMovementComponent::BeginPerch()
{
    if (!OwnerCharacter)
    {
        OwnerCharacter = Cast<ACharacter>(GetOwner());
    }

    if (!OwnerCharacter) return;

    StopFlightTick();
    bIsFlying = false;

    UCharacterMovementComponent* Move = OwnerCharacter->GetCharacterMovement();
    if (Move)
    {
        Move->SetMovementMode(MOVE_None);
    }

    SetPhase(EPhase::Idle);
    //UE_LOG(LogTemp, Warning, TEXT("[BeginPerch] Bird has perched and stopped flying."));
}

// SetMovementTOPatrol
void UFlyingMovementComponent::BeginWalkFromPerch()
{
    if (!OwnerCharacter)
    {
        OwnerCharacter = Cast<ACharacter>(GetOwner());
    }

    if (!OwnerCharacter) return;

    StopFlightTick();
    bIsFlying = false;

    UCharacterMovementComponent* Move = OwnerCharacter->GetCharacterMovement();
    if (Move)
    {
        Move->SetMovementMode(MOVE_Walking);
        Move->GravityScale = 1.0f;
    }

    SetPhase(EPhase::Idle);
    //UE_LOG(LogTemp, Warning, TEXT("[BeginWalkFromPerch] Bird switched to walking mode."));
}

// LandWhenCloseToGroundTarget
bool UFlyingMovementComponent::ShouldLandNearGroundTarget(float Threshold) const
{
    if (!OwnerCharacter || !GroundTarget) return false;

    const float Distance = FVector::Dist(OwnerCharacter->GetActorLocation(), GroundTarget->GetActorLocation());
    return Distance <= Threshold;
}

// ShuffleSkyTargetWhenNearCurrent
void UFlyingMovementComponent::UpdateSkyTargetIfNear(float Threshold)
{
    if (!OwnerCharacter || !SkyTargets.Num()) return;

    if (!CurrentSkyTarget)
    {
        CurrentSkyTarget = SkyTargets[FMath::RandRange(0, SkyTargets.Num() - 1)];
        return;
    }

    const float Distance = FVector::Dist(OwnerCharacter->GetActorLocation(), CurrentSkyTarget->GetActorLocation());
    if (Distance <= Threshold)
    {
        AActor* NewTarget = nullptr;
        do
        {
            NewTarget = SkyTargets[FMath::RandRange(0, SkyTargets.Num() - 1)];
        }
        while (NewTarget == CurrentSkyTarget && SkyTargets.Num() > 1);

        CurrentSkyTarget = NewTarget;

        //UE_LOG(LogTemp, Warning, TEXT("[UpdateSkyTargetIfNear] New SkyTarget assigned."));
    }
}

// FlyToARandomSkyTarget
void UFlyingMovementComponent::UpdateFlightTowardSkyTarget(float DeltaTime)
{
    if (!OwnerCharacter || !CurrentSkyTarget) return;
    
    FVector ToTarget = (CurrentSkyTarget->GetActorLocation() - OwnerCharacter->GetActorLocation()).GetSafeNormal();
    
    FVector AdjustedDir = DetectObstacleDirection(ToTarget, /*bDoDownTrace=*/ true);
    FVector CurrentForward = OwnerCharacter->GetActorForwardVector();
    
    FQuat CurrentRotation = OwnerCharacter->GetActorQuat();
    FQuat TargetRotation  = FQuat::FindBetweenNormals(CurrentForward, AdjustedDir);
    FQuat NewRotation     = FQuat::Slerp(CurrentRotation, TargetRotation * CurrentRotation, DeltaTime * 2.0f);
    OwnerCharacter->SetActorRotation(NewRotation);
    
    FVector ForwardVelocity = AdjustedDir * MoveSpeed;
    OwnerCharacter->GetCharacterMovement()->Velocity = ForwardVelocity;
    
    FVector GravityAdjust = FVector::UpVector * -200.f;
    OwnerCharacter->GetCharacterMovement()->Velocity += GravityAdjust * DeltaTime;
    
    // if (!OwnerCharacter || !CurrentSkyTarget) return;
    //
    // FVector ToTarget = (CurrentSkyTarget->GetActorLocation() - OwnerCharacter->GetActorLocation()).GetSafeNormal();
    // FVector CurrentForward = OwnerCharacter->GetActorForwardVector();
    //
    // float TurnAngle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(CurrentForward, ToTarget)));
    // FVector TurnAxis = FVector::CrossProduct(CurrentForward, ToTarget).GetSafeNormal();
    //
    // FQuat CurrentRotation = OwnerCharacter->GetActorQuat();
    // FQuat TargetRotation = FQuat::FindBetweenNormals(CurrentForward, ToTarget);
    // FQuat NewRotation = FQuat::Slerp(CurrentRotation, TargetRotation * CurrentRotation, DeltaTime * 2.0f);
    // OwnerCharacter->SetActorRotation(NewRotation);
    //
    // FVector ForwardVelocity = OwnerCharacter->GetActorForwardVector() * MoveSpeed;
    // OwnerCharacter->GetCharacterMovement()->Velocity = ForwardVelocity;
    //
    // FVector GravityAdjust = FVector::UpVector * -200.f;
    // OwnerCharacter->GetCharacterMovement()->Velocity += GravityAdjust * DeltaTime;
}

// BTService_Bird_ObstacleAvoidance
FVector UFlyingMovementComponent::DetectObstacleDirection(const FVector& CurDir, bool bDoDownTrace) const
{
    if (!OwnerCharacter || !bDoDownTrace)
    {
        return CurDir;
    }
    
    const FVector ActorLoc = OwnerCharacter->GetActorLocation();
    const FVector TraceStart(ActorLoc.X, ActorLoc.Y, ActorLoc.Z - 120.0f);
    
    const FVector TraceEnd = TraceStart + FVector::UpVector * (-ObstacleAvoidanceDownDistance);
    
    FHitResult HitResult;
    FCollisionQueryParams Params(TEXT("DetectGround"), true, OwnerCharacter);
    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, Params);

    if (bHit)
    {
        return (CurDir + FVector::UpVector * VerticalThrustAmount).GetSafeNormal();
    }

    return CurDir;
}

// BTTask_Bird_ZeroOutPitchAndRoll
void UFlyingMovementComponent::ResetPitchAndRoll()
{
    AActor* Owner = GetOwner();
    if (!Owner) return;

    FRotator CurrentRot = Owner->GetActorRotation();
    FRotator UprightRot(0.0f, CurrentRot.Yaw, 0.0f);
    Owner->SetActorRotation(UprightRot);

    if (AAIController* AICont = Cast<AAIController>(Owner->GetInstigatorController()))
    {
        AICont->StopMovement();
    }
}

void UFlyingMovementComponent::ChooseRandomGroundTarget()
{
    if (GroundTargets.Num() == 0)
    {
        UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("GroundTarget")), GroundTargets);
    }
    
    if (GroundTargets.Num() > 0)
    {
        int32 Index = RandomStream.RandRange(0, GroundTargets.Num() - 1);
        GroundTarget = GroundTargets[Index];
    }
}

void UFlyingMovementComponent::MoveToGroundTarget(float DeltaTime)
{
    if (!OwnerCharacter || !GroundTarget) return;
    
    FVector ActorLoc = OwnerCharacter->GetActorLocation();
    FVector TargetLoc = GroundTarget->GetActorLocation();
    FVector ToTarget = (TargetLoc - ActorLoc).GetSafeNormal();
    
    FVector LocalDir = OwnerCharacter->GetTransform().InverseTransformVector(ToTarget);
    LocalDir.Normalize();
    
    float TargetPitchInput = -LocalDir.Z;
    CurrentPitch = FMath::FInterpTo(CurrentPitch, TargetPitchInput, DeltaTime, 2.0f);
    
    float TargetYawInput = (FMath::Abs(LocalDir.Y) > 0.5f) ? LocalDir.Y : 0.0f;
    CurrentYaw = FMath::FInterpTo(CurrentYaw, TargetYawInput, DeltaTime, 2.0f);
    
    float TargetRollInput = LocalDir.Y * -3.0f;
    CurrentRoll = FMath::FInterpTo(CurrentRoll, TargetRollInput, DeltaTime, 2.0f);
    
    FRotator DeltaRot = FRotator(
        2.0f * DeltaTime * CurrentPitch,
        CurrentYaw * 0.5f * DeltaTime,
        CurrentRoll * DeltaTime
    );
    OwnerCharacter->AddActorLocalRotation(DeltaRot);
    
    FVector ForwardMove = FVector(MoveSpeed * DeltaTime, 0.0f, 0.0f);
    OwnerCharacter->AddActorLocalOffset(ForwardMove, true);
}
