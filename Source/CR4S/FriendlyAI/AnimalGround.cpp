#include "AnimalGround.h"
#include "Component/AIJumpComponent.h"
#include "../Gimmick/Components/InteractableComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AAnimalGround::AAnimalGround()
{
	AIJumpComponent = CreateDefaultSubobject<UAIJumpComponent>(TEXT("AIJumpComponent"));
	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));
	InteractableComponent->SetActive(false);
}

void AAnimalGround::BeginPlay()
{
	Super::BeginPlay();

	if (InteractableComponent)
	{
		InteractableComponent->OnTryInteract.AddUniqueDynamic(this, &ABaseAnimal::OnInteract);
	}
	
	if (AIJumpComponent)
	{
		AIJumpComponent->SetJumpPower(JumpPower);
	}

	UCharacterMovementComponent* Move = GetCharacterMovement();
	if (Move)
	{
		Move->bOrientRotationToMovement = true;
		Move->RotationRate = FRotator(0.f, 120.f, 0.f);
	}
}

#pragma region Stun
void AAnimalGround::ApplyStun(float Amount)
{
	Super::ApplyStun(Amount);

	if (!bIsStunned) return;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	if (GetController())
	{
		GetController()->StopMovement();
	}

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	FName BoneName = GetMesh()->GetSocketBoneName(TEXT("TS"));
	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetAllBodiesBelowSimulatePhysics(BoneName, true, true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AAnimalGround::RecoverFromStun()
{
	Super::RecoverFromStun();

	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	GetMesh()->bBlendPhysics = true;
	GetMesh()->SetAllBodiesSimulatePhysics(false);
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}
#pragma endregion

#pragma region Attack
void AAnimalGround::PerformChargeAttack()
{
	// if (StatsRow)
	// {
	// 	LaunchCharacter(GetActorForwardVector() * StatsRow->ChargeSpeed, true, true);
	// 	//GetCharacterMovement()->Velocity = GetActorForwardVector() * StatsRow->ChargeSpeed;
	// } 
}
#pragma endregion
