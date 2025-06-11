#include "AnimalFlying.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Gimmick/Components/InteractableComponent.h"
#include "Component/FlyingMovementComponent.h"

AAnimalFlying::AAnimalFlying()
{
	FlyingComp = CreateDefaultSubobject<UFlyingMovementComponent>(TEXT("FlyingMovementComponent"));
	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));
	InteractableComponent->SetActive(false);
}

void AAnimalFlying::BeginPlay()
{
	Super::BeginPlay();
	
	if (InteractableComponent)
	{
		InteractableComponent->OnTryInteract.AddUniqueDynamic(this, &ABaseAnimal::OnInteract);
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
		SkelMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
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
		SkelMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
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