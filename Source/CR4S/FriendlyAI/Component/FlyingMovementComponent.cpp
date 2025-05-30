#include "FlyingMovementComponent.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UFlyingMovementComponent::UFlyingMovementComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    
    AscendSpeed = 300.f;
    ForwardSpeed = 300.f;
    MinPeak = 600.f;
    CruiseMinTime = 3.f;

    TurnAccel = 4.f;
    MaxYawRate = 180.f;

    TraceInterval = 0.02f;
    TraceLen = 800.f;
    MaxSearch = 3;
    SideTraceLen = 1200.f;
    LandingTraceLen = 2000.f;
}

void UFlyingMovementComponent::BeginPlay()
{
    Super::BeginPlay();
    //RandomStream.Initialize(GetUniqueID());
    //RandomStream.Initialize(GetOwner()->GetUniqueID());
    RandomStream.Initialize((int32)(PTRINT(this)));
}

void UFlyingMovementComponent::InitializeComponent()
{
    Super::InitializeComponent();
    RandomStream.Initialize((int32)(PTRINT(this)));
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
    FVector ModifiedDest = Dest
        + RandomStream.VRand() * RandomStream.FRandRange(0.f, JitterRadius);
        
    OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (!OwnerCharacter)
    {
        return;
    }
    
    TargetLocation = ModifiedDest;
    
    TargetZ       = OwnerCharacter->GetActorLocation().Z + MinPeak;
    CruiseElapsed = 0.f;
    SearchCount   = 0;
    Phase         = EPhase::Ascend;
    CurrentDir    = OwnerCharacter->GetActorForwardVector().GetSafeNormal();

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
        // UE_LOG(LogTemp, Log, TEXT("Success =(%s)"),
        //        *OwnerCharacter->GetActorLocation().ToString());
    }
}

void UFlyingMovementComponent::TickFlight()
{
    if (!OwnerCharacter)
    {
        return;
    }

    FVector Cur = OwnerCharacter->GetActorLocation();
    FVector DesiredDir = ChooseDir(Cur);

    const float JitterStrength = 1.f;  
    FVector Jitter = RandomStream.VRand() * JitterStrength;
    DesiredDir = (DesiredDir + Jitter).GetSafeNormal();
    
    CurrentDir = FMath::VInterpTo(CurrentDir, DesiredDir, TraceInterval, TurnAccel).GetSafeNormal();
    
    switch (Phase)
    {
    case EPhase::Ascend:
        Cur.Z += AscendSpeed * TraceInterval;
        if (Cur.Z >= TargetZ)
        {
            Cur.Z = TargetZ;
            Phase = EPhase::Cruise;
        }
        break;

    case EPhase::Cruise:
        CruiseElapsed += TraceInterval;
        {
            const float BobAmp = 10.f;
            const float BobHz  = 4.f;
            float BobOffset = FMath::Sin(CruiseElapsed * PI * BobHz) * BobAmp;
            Cur.Z = TargetZ + BobOffset;
        }
        if (CruiseElapsed >= CruiseMinTime)
        {
            Phase = EPhase::Descend;
        }
        break;

    case EPhase::Descend:
        Cur.Z -= AscendSpeed * TraceInterval;

        if (CanLand(Cur))
        {
            StopMovement();
            return;
        }

        float HitZ = GroundZAt(Cur);
        break;
    }
    
    Cur += CurrentDir * ForwardSpeed * TraceInterval;
    bool bMoved = OwnerCharacter->SetActorLocation(Cur, true);
    
    FRotator TargetRot = CurrentDir.Rotation();
    FRotator NewRot = FMath::RInterpConstantTo(
        OwnerCharacter->GetActorRotation(), TargetRot,
        TraceInterval, MaxYawRate);
    if (AController* C = OwnerCharacter->GetController())
    {
        C->SetControlRotation(NewRot);
    }
    OwnerCharacter->SetActorRotation(NewRot);
    
    DrawDebugSphere(GetWorld(), Cur, 12.f, 8,
                FColor::Blue, false, TraceInterval, 0, 2.f);
}

FVector UFlyingMovementComponent::ChooseDir(const FVector& CurPos) const
{
    FVector Fwd  = CurrentDir;
    FVector L45  = Fwd.RotateAngleAxis(-45.f, FVector::UpVector);
    FVector R45  = Fwd.RotateAngleAxis( 45.f, FVector::UpVector);
    FVector Up   = FVector::UpVector;
    FVector Down = -Up;
    const FVector Cands[5] = { Fwd, L45, R45, Up, Down };
    
    TArray<FVector> ValidDirs;
    const float CheckDist  = 500.f;
    const float DownLen    = 1000.f;
    for (const FVector& Dir : Cands)
    {
        FVector TestPt     = CurPos + Dir * CheckDist;
        FVector TraceStart = TestPt + FVector(0,0,50);
        FVector TraceEnd   = TestPt - FVector(0,0,DownLen);

        FHitResult FloorHit;
        if (!GetWorld()->LineTraceSingleByChannel(FloorHit, TraceStart, TraceEnd, ECC_Visibility))
        {
            continue;
        }
        
        FVector Start = CurPos + Dir * 50.f;
        float Len     = (Dir.Equals(L45,0.01f)||Dir.Equals(R45,0.01f)) ? SideTraceLen : TraceLen;
        FVector End   = Start + Dir * Len;
        FHitResult Hit;
        bool bHit     = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility);

        DrawDebugLine(GetWorld(), Start, End,
                      bHit ? FColor::Red : FColor::Green,
                      false, TraceInterval, 0, 2.f);

        ValidDirs.Add(Dir);
    }
    const FVector* UseCands = (ValidDirs.Num() > 0)
        ? ValidDirs.GetData()
        : Cands;
    int32 NumCands = (ValidDirs.Num() > 0)
        ? ValidDirs.Num()
        : UE_ARRAY_COUNT(Cands);
    
    float   BestDist = -1.f;
    FVector BestDir  = Fwd;
    for (int32 i = 0; i < NumCands; ++i)
    {
        const FVector& Dir = UseCands[i];
        FVector Start = CurPos + Dir * 50.f;
        float Len = (Dir.Equals(L45, 0.01f) || Dir.Equals(R45, 0.01f))
            ? SideTraceLen : TraceLen;
        FVector End = Start + Dir * Len;

        FHitResult Hit;
        bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility);

        float Dist = bHit ? Hit.Distance : TraceLen;
        if (Dist > BestDist)
        {
            BestDist = Dist;
            BestDir  = Dir;
        }
    }
    return BestDir;
}


float UFlyingMovementComponent::GroundZAt(const FVector& Pos) const
{
    FVector End = Pos - FVector(0, 0, LandingTraceLen);
    FHitResult Hit;
    bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Pos, End, ECC_Visibility);

    DrawDebugLine(GetWorld(), Pos, End,
                  bHit ? FColor::Red : FColor::Green,
                  false, 0.f, 0, 2.f);

    return bHit ? Hit.Location.Z : -1.f;
}

bool UFlyingMovementComponent::CanLand(const FVector& CurPos) const
{
    float Z = GroundZAt(CurPos + CurrentDir * 50.f);
    if (Z < 0.f)
    {
        return false;
    }

    bool bCanLand = CurPos.Z - Z < 150.f;

    DrawDebugCircle(GetWorld(), FVector(CurPos.X, CurPos.Y, Z),
                40.f, 12, bCanLand ? FColor::Green : FColor::Red,
                false, TraceInterval, 0, 2.f,
                FVector::YAxisVector, FVector::ZAxisVector, false);
    return bCanLand;
}