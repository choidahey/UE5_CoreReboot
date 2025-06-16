#include "FlyingAnimalAnimInstance.h"
#include "FriendlyAI/AnimalFlying.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

UFlyingAnimalAnimInstance::UFlyingAnimalAnimInstance()
{
    OwningAnimal = nullptr;
}

void UFlyingAnimalAnimInstance::NativeBeginPlay()
{
    Super::NativeBeginPlay();
    
    OwningAnimal = Cast<AAnimalFlying>(TryGetPawnOwner());
}

void UFlyingAnimalAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime);
    
    if (!OwningAnimal) return;
    
    UpdateStateParameters();
    UpdateMovementParameters();
    UpdateFlyingParameters();
}

void UFlyingAnimalAnimInstance::UpdateStateParameters()
{
    if (!OwningAnimal) return;
    
    AreWeWalking = OwningAnimal->GetAreWeWalking();
    AreWeFlying = OwningAnimal->GetAreWeFlying();
    PlayTakeOffAnimation = OwningAnimal->GetPlayTakeOffAnimation();
    
    if (AAIController* AIController = Cast<AAIController>(OwningAnimal->GetController()))
    {
        if (UBlackboardComponent* BB = AIController->GetBlackboardComponent())
        {
            AreWePerched = BB->GetValueAsBool("AreWePerched");
        }
    }
    
    if (UCharacterMovementComponent* MoveComp = OwningAnimal->GetCharacterMovement())
    {
        IsInAir = (MoveComp->MovementMode == MOVE_Flying || MoveComp->MovementMode == MOVE_Falling);
    }
    
    PlayTakeOffAnimation = OwningAnimal->GetPlayTakeOffAnimation();
    FlyToPerch = OwningAnimal->GetFlyToPerch();
    LandOnPerch = OwningAnimal->GetLandOnPerch();
    
    RandomIdleAnimation = OwningAnimal->GetRandomIdleAnimation();
}

void UFlyingAnimalAnimInstance::UpdateMovementParameters()
{
    if (!OwningAnimal) return;
    
    UCharacterMovementComponent* MoveComp = OwningAnimal->GetCharacterMovement();
    if (!MoveComp) return;
    
    FVector Velocity = MoveComp->Velocity;
    FVector LocalVelocity = OwningAnimal->GetActorRotation().UnrotateVector(Velocity);
    
    Direction = LocalVelocity.Y;
    
    FVector HorizontalVelocity = FVector(Velocity.X, Velocity.Y, 0.0f);
    WalkingSpeed = HorizontalVelocity.Size();
    
    if (AreWeWalking)
    {
        PlayIdleLoop = (WalkingSpeed < 20.0f);
    }
    else
    {
        PlayIdleLoop = false;
    }
}

void UFlyingAnimalAnimInstance::UpdateFlyingParameters()
{
    if (!OwningAnimal) return;
    
    UCharacterMovementComponent* MoveComp = OwningAnimal->GetCharacterMovement();
    if (MoveComp)
    {
        FVector Velocity = MoveComp->Velocity;
        float VerticalSpeed = FMath::Abs(Velocity.Z);
        float HorizontalSpeed = FVector(Velocity.X, Velocity.Y, 0.0f).Size();
        
        FlapFast = (VerticalSpeed > 300.0f) || (HorizontalSpeed > 800.0f);
        
        FlyingDownward = (Velocity.Z < -200.0f);
    }
}