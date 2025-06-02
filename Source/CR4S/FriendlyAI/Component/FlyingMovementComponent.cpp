#include "FlyingMovementComponent.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h" 

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
    if (ACharacter* OwnerChar = Cast<ACharacter>(GetOwner()))
    {
        if (UCharacterMovementComponent* CharMove = OwnerChar->GetCharacterMovement())
        {
            CharMove->GravityScale = 0.f;
        }
    }
    RandomStream.Initialize((int32)(PTRINT(this)));
}

void UFlyingMovementComponent::InitializeComponent()
{
    Super::InitializeComponent();
    RandomStream.Initialize((int32)(PTRINT(this)));
}

void UFlyingMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    FVector Cur = GetOwner()->GetActorLocation();

    {
        FVector BaseDir = ChooseOptimalDirection(Cur);
        FVector Jittered = CalculateJitter(BaseDir);
        InterpolateDirection(Jittered);
    }

    FVector MoveDir = FMath::VInterpTo(CurrentDir, FlockDir, DeltaTime, TurnAccel).GetSafeNormal();

    Cur += MoveDir * FlightSpeed * DeltaTime;
    GetOwner()->SetActorLocation(Cur);

    DrawMovementDebug(Cur, DeltaTime);
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
    TargetZ        = OwnerCharacter->GetActorLocation().Z + MinPeak;
    CruiseElapsed  = 0.f;
    SearchCount    = 0;
    Phase          = EPhase::Ascend;
    CurrentDir     = OwnerCharacter->GetActorForwardVector().GetSafeNormal();

    auto* Move = OwnerCharacter->GetCharacterMovement();
    Move->SetMovementMode(MOVE_Flying);
    OwnerCharacter->bUseControllerRotationYaw = false;
    Move->bOrientRotationToMovement         = false;

    GetWorld()->GetTimerManager().SetTimer(
        FlightTimer, this, &UFlyingMovementComponent::TickFlight,
        TraceInterval, true, 0.f);
}

void UFlyingMovementComponent::MoveToActor(AActor* TargetActor)
{
    MoveToLocation(FVector::ZeroVector);
}

void UFlyingMovementComponent::StopMovement()
{
    GetWorld()->GetTimerManager().ClearTimer(FlightTimer);
    if (OwnerCharacter)
    {
        OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
    }
}

void UFlyingMovementComponent::TickFlight()
{
    if (!OwnerCharacter)
    {
        return;
    }

    FVector Cur = OwnerCharacter->GetActorLocation();

    UpdateDirection(Cur);
    HandlePhase(TraceInterval, Cur);
    ApplyMovement(Cur, TraceInterval);
    ApplyRotation(TraceInterval);
    DrawMovementDebug(Cur, TraceInterval);
}

void UFlyingMovementComponent::UpdateDirection(const FVector& Cur)
{
    FVector BaseDir   = DetectObstacleDirection(Cur);
    FVector Jittered  = CalculateJitter(BaseDir);
    InterpolateDirection(Jittered);
}

FVector UFlyingMovementComponent::DetectObstacleDirection(const FVector& Cur) const
{
    FVector PrevDir = CurrentDir;
    FVector DesiredDir = PrevDir;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());
    
    FVector ForwardStart = Cur;
    FVector ForwardEnd   = Cur + PrevDir * TraceLen;
    FHitResult HitForward;
    if (GetWorld()->LineTraceSingleByChannel(HitForward, ForwardStart, ForwardEnd, ECC_WorldStatic, Params))
    {
        float YawOffset = (RandomStream.FRand() < 0.5f ? -15.f : 15.f);
        DesiredDir = (PrevDir.RotateAngleAxis(YawOffset, FVector::UpVector)).GetSafeNormal();
    }

    FVector RightVec = FVector::CrossProduct(FVector::UpVector, PrevDir).GetSafeNormal();
    for (int32 Side : { -1, 1 })
    {
        FVector SideStart = Cur;
        FVector SideEnd   = Cur + RightVec * Side * SideTraceLen;
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
        FVector TestPt     = Cur + Dir * CheckDist;
        FVector TraceStart = TestPt + FVector(0,0,50);
        FVector TraceEnd   = TestPt - FVector(0,0,DownLen);

        FHitResult FloorHit;
        if (!GetWorld()->LineTraceSingleByChannel(FloorHit, TraceStart, TraceEnd, ECC_Visibility))
        {
            continue;
        }

        FVector Start2 = Cur + Dir * 50.f;
        float Len = (Dir.Equals(L45, 0.01f) || Dir.Equals(R45, 0.01f)) ? SideTraceLen : TraceLen;
        FVector End2 = Start2 + Dir * Len;

        FHitResult Hit2;
        bool bHit2 = GetWorld()->LineTraceSingleByChannel(Hit2, Start2, End2, ECC_Visibility);
        if (!bHit2)
        {
            ValidDirs.Add(Dir);
        }
    }

    if (ValidDirs.Num() > 0)
    {
        DesiredDir = ValidDirs[RandomStream.RandRange(0, ValidDirs.Num()-1)].GetSafeNormal();
    }
    
    return FMath::VInterpTo(PrevDir, DesiredDir, TraceInterval, TurnAccel).GetSafeNormal();
}

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
    return bCanLand;
}

void UFlyingMovementComponent::ApplyMovement(FVector& Cur, float DeltaTime)
{
    Cur += CurrentDir * ForwardSpeed * DeltaTime;
    OwnerCharacter->SetActorLocation(Cur, true);
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