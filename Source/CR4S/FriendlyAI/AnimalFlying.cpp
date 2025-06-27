#include "AnimalFlying.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Gimmick/Components/InteractableComponent.h"
#include "Component/FlyingMovementComponent.h"
#include "Components/CapsuleComponent.h"

AAnimalFlying::AAnimalFlying()
{
	//FlyingComp = CreateDefaultSubobject<UFlyingMovementComponent>(TEXT("FlyingMovementComponent"));

	//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AAnimalFlying::OnCapsuleHit);
}

void AAnimalFlying::BeginPlay()
{
	Super::BeginPlay();

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		BlackboardComponent = AIController->FindComponentByClass<UBlackboardComponent>();
	}

	if (bBeginLevelFlying)
	{
		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			MoveComp->SetMovementMode(MOVE_Flying);
		}

		if (bBeginLevelFlying && BlackboardComponent)
		{
			BlackboardComponent->SetValueAsBool("AreWeWalking", false);
			BlackboardComponent->SetValueAsBool("AreWePerched", false);
			BlackboardComponent->SetValueAsBool("AreWeFlying", true);
		}
		if (UCapsuleComponent* Capsule = GetCapsuleComponent())
		{
			if (bDisableCollisionDuringFlight)
			{
				//Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				//Capsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			}
			else
			{
				//Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			}
		}

		GetWorldTimerManager().SetTimer(
		AnimationUpdateTimer,
		this,
		&AAnimalFlying::UpdateAnimationParametersFromBlackboard,
		0.1f,
		true
	);
	}
	else
	{
		if (bBeginLevelWalking && BlackboardComponent)
		{
			if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
			{
				MoveComp->SetMovementMode(MOVE_Walking);
			}

			BlackboardComponent->SetValueAsBool("AreWeWalking", true);
			BlackboardComponent->SetValueAsBool("AreWePerched", true);
			BlackboardComponent->SetValueAsBool("AreWeFlying", false);
		}
		else
		{
			if (bBeginLevelPerched && BlackboardComponent)
			{
				if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
				{
					MoveComp->SetMovementMode(MOVE_Flying);
				}

				BlackboardComponent->SetValueAsBool("AreWeWalking", false);
				BlackboardComponent->SetValueAsBool("AreWePerched", true);
				BlackboardComponent->SetValueAsBool("AreWeFlying", false);
				BlackboardComponent->SetValueAsBool("PlayPerchIdle", true);
			}
		}
	}

}

void AAnimalFlying::OnCapsuleHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bDisableCollisionDuringFlight)
	{
		return;
	}

	if (BlackboardComponent &&
	!BlackboardComponent->GetValueAsBool("AreWeWalking") &&
	!BlackboardComponent->GetValueAsBool("IsLookingForPerch") &&
	!BlackboardComponent->GetValueAsBool("AreWePerched") &&
	!BlackboardComponent->GetValueAsBool("FlyToPerch"))
	{
		if (OtherComp != GetMesh())
		{
			SetMovementToWalking();
			GetCharacterMovement()->GravityScale = 0.4f;

			BlackboardComponent->SetValueAsBool("AreWeWalking", true);
			BlackboardComponent->SetValueAsBool("AreWeFlying", false);
			BlackboardComponent->SetValueAsBool("AreWePerched", false);
			BlackboardComponent->SetValueAsBool("TimeToLand", false);
			BlackboardComponent->SetValueAsBool("IsLookingForPerch", false);
		}
	}
}

void AAnimalFlying::SetMovementToFlying()
{
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		bAreWeFlying = true;
		bAreWeWalking = false;
		MoveComp->SetMovementMode(MOVE_Flying);
	}
}

void AAnimalFlying::SetMovementToWalking()
{
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		bAreWeFlying = false;
		bAreWeWalking = true;
		IdleAnimIndex = FMath::RandRange(0, 2);
		MoveComp->SetMovementMode(MOVE_Walking);
	}
}

#pragma region Stun
void AAnimalFlying::ApplyStun(float Amount)
{
	Super::ApplyStun(Amount);

	if (!bIsStunned) return;

	if (USkeletalMeshComponent* SkelMesh = GetMesh())
	{
		SkelMesh->SetAllBodiesSimulatePhysics(true);
		SkelMesh->SetSimulatePhysics(true);
		//SkelMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->DisableMovement();
	}
}

void AAnimalFlying::RecoverFromStun()
{
	Super::RecoverFromStun();

	if (USkeletalMeshComponent* SkelMesh = GetMesh())
	{
		SkelMesh->SetSimulatePhysics(false);
		SkelMesh->SetAllBodiesSimulatePhysics(false);
		//SkelMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		SkelMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	}

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	}
}
#pragma endregion

#pragma region Attack
void AAnimalFlying::PerformChargeAttack()
{
	Super::PerformChargeAttack();
	// if (StatsRow)
	// {
	// 	LaunchCharacter(GetActorForwardVector() * StatsRow->ChargeSpeed, true, true);
	// 	//GetCharacterMovement()->Velocity = GetActorForwardVector() * StatsRow->ChargeSpeed;
	// } 
}
#pragma endregion

void AAnimalFlying::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	if (FlyingComp && FlyingComp->GetPhase() != UFlyingMovementComponent::EPhase::Idle)
	{
		if (FlyingComp->GetPhase() != UFlyingMovementComponent::EPhase::Idle)
		{
			GetCharacterMovement()->StopMovementImmediately();
			FlyingComp->SetPhase(UFlyingMovementComponent::EPhase::Idle);
		}
	}
}

void AAnimalFlying::LiftOff()
{
	if (FlyingComp)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		FlyingComp->SetPhase(UFlyingMovementComponent::EPhase::Ascend);
	}
}

void AAnimalFlying::Land()
{
	if (FlyingComp)
	{
		FlyingComp->SetPhase(UFlyingMovementComponent::EPhase::Descend);
	}
}

void AAnimalFlying::UpdateAnimationParametersFromBlackboard()
{
	if (!BlackboardComponent) return;
	
	bPlayPerchedIdle = BlackboardComponent->GetValueAsBool("PlayPerchIdle");
	bLandOnPerch = BlackboardComponent->GetValueAsBool("PlayPerchIdle");
	bFlyToPerch = BlackboardComponent->GetValueAsBool("FlyToPerch");
	bAreWeWalking = BlackboardComponent->GetValueAsBool("AreWeWalking");
	bAreWeFlying = BlackboardComponent->GetValueAsBool("AreWeFlying");
	bFlapFast = BlackboardComponent->GetValueAsBool("FlapFast");
	bAreWePerched = BlackboardComponent->GetValueAsBool("AreWePerched");

	if (bAreWeWalking && !bAreWeFlying)
	{
		bPlayIdleLoop = true;
	}
	else
	{
		bPlayIdleLoop = false;
	}
}
