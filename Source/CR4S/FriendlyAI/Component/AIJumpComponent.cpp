#include "AIJumpComponent.h"
#include "GameFramework/Character.h"
#include "NavLinkCustomComponent.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "../Component/GroundMovementComponent.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Navigation/NavLinkProxy.h"

UAIJumpComponent::UAIJumpComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    JumpPower = 600.f;
}

void UAIJumpComponent::BeginPlay()
{
    Super::BeginPlay();
    OwnerCharacter = Cast<ACharacter>(GetOwner());
}

void UAIJumpComponent::OnEQSQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
    if (!QueryInstance || QueryStatus != EEnvQueryStatus::Success) return;

    ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
    if (!OwnerChar) return;

    FVector Start = OwnerChar->GetActorLocation();
    FVector End = QueryInstance->GetResultsAsLocations()[0];

    const float Gravity   = -GetWorld()->GetGravityZ();
    const float Time      = (2 * JumpPower) / Gravity;
    const float MaxHeight = (JumpPower * Time) - 0.5f * Gravity * FMath::Square(Time);
    float DeltaZ = End.Z - Start.Z;

    if (DeltaZ <= MaxHeight)
    {
        FVector Dir2D = (End - Start).GetSafeNormal2D();
        float HorizontalSpeed = (End - Start).Size2D() / Time;
        FVector LaunchVelocity = Dir2D * HorizontalSpeed;
        LaunchVelocity.Z = JumpPower;
        OwnerChar->LaunchCharacter(LaunchVelocity, true, true);
    }
    else
    {
        if (auto* GM = OwnerChar->FindComponentByClass<UGroundMovementComponent>())
        {
            GM->MoveToLocation(End);
        }
    }
}


void UAIJumpComponent::OnCustomLinkReached(UNavLinkCustomComponent* SmartLinkComp, UObject* PathComp, const FVector& DestPoint)
{
    if (OwnerCharacter)
    {
        OwnerCharacter->LaunchCharacter(PendingLaunchVelocity, true, true);
    }
}

void UAIJumpComponent::TryJump()
{
    if (!OwnerCharacter)
    {
        return;
    }
    if (OwnerCharacter->CanJump())
    {
        OwnerCharacter->Jump();
    }
}
