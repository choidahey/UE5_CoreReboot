#include "AnimalMonster.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Component/AIJumpComponent.h"
#include "MonsterAI/BossMonster/Season/SeasonBossMonster.h"

AAnimalMonster::AAnimalMonster()
{
	AIJumpComponent = CreateDefaultSubobject<UAIJumpComponent>(TEXT("AIJumpComponent"));
}

void AAnimalMonster::BeginPlay()
{
	Super::BeginPlay();
	UCharacterMovementComponent* Move = GetCharacterMovement();
	if (Move)
	{
		Move->bOrientRotationToMovement = true;
		Move->RotationRate = FRotator(0.f, 120.f, 0.f);
	}

	if (AIJumpComponent)
	{
		AIJumpComponent->SetJumpPower(JumpPower);
	}
}

#pragma region Stun
void AAnimalMonster::ApplyStun(float Amount)
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

void AAnimalMonster::RecoverFromStun()
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
void AAnimalMonster::PerformChargeAttack()
{
}
#pragma endregion
