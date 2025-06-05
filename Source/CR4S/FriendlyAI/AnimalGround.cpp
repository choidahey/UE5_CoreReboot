#include "AnimalGround.h"
#include "Component/AIJumpComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AAnimalGround::AAnimalGround()
{
	AIJumpComponent = CreateDefaultSubobject<UAIJumpComponent>(TEXT("AIJumpComponent"));
}

void AAnimalGround::BeginPlay()
{
	Super::BeginPlay();
	
	if (AIJumpComponent)
	{
		AIJumpComponent->SetJumpPower(JumpPower);
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
