#include "BTT_FAChargeAttack.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "FriendlyAI/AnimalFlying.h"

UBTT_FAChargeAttack::UBTT_FAChargeAttack()
{
	NodeName = TEXT("FA Charge Attack");
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

uint16 UBTT_FAChargeAttack::GetInstanceMemorySize() const
{
	return sizeof(FBTTChargeAttackMemory);
}

EBTNodeResult::Type UBTT_FAChargeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FBTTChargeAttackMemory* Memory = reinterpret_cast<FBTTChargeAttackMemory*>(NodeMemory);
	if (!Memory) return EBTNodeResult::Failed;

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn) return EBTNodeResult::Failed;
	
	APawn* Player = UGameplayStatics::GetPlayerPawn(ControlledPawn->GetWorld(), 0);
	if (!Player) return EBTNodeResult::Failed;

	FVector CurrentLocation = ControlledPawn->GetActorLocation();
	FVector PlayerLocation = Player->GetActorLocation();
	
	FHitResult HitResult;
	FCollisionQueryParams TraceParams(FName(TEXT("ChargeTrace")), true, ControlledPawn);
	TraceParams.AddIgnoredActor(Player);
	TraceParams.bTraceComplex = false;

	bool bHit = ControlledPawn->GetWorld()->LineTraceSingleByChannel(
		HitResult,
		CurrentLocation,
		PlayerLocation,
		ECC_Visibility,
		TraceParams
	);
	
	if (bHit)
	{
		return EBTNodeResult::Failed;
	}
	
	Memory->TargetLocation = PlayerLocation;
	Memory->bIsCharging = true;
	
	if (AAnimalFlying* FlyingAnimal = Cast<AAnimalFlying>(ControlledPawn))
	{
		if (UCharacterMovementComponent* MoveComp = FlyingAnimal->GetCharacterMovement())
		{
			FVector Direction = (Memory->TargetLocation - CurrentLocation).GetSafeNormal();
			MoveComp->Velocity = Direction * ChargeSpeed;
		}
	}

	return EBTNodeResult::InProgress;
}

void UBTT_FAChargeAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FBTTChargeAttackMemory* Memory = reinterpret_cast<FBTTChargeAttackMemory*>(NodeMemory);
	if (!Memory || !Memory->bIsCharging) return;

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return;

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn) return;

	FVector CurrentLocation = ControlledPawn->GetActorLocation();
	float DistanceToTarget = FVector::Dist(CurrentLocation, Memory->TargetLocation);
	
	if (DistanceToTarget <= ArrivalThreshold)
	{
		if (AAnimalFlying* FlyingAnimal = Cast<AAnimalFlying>(ControlledPawn))
		{
			if (UCharacterMovementComponent* MoveComp = FlyingAnimal->GetCharacterMovement())
			{
				MoveComp->Velocity = FVector::ZeroVector;
			}
		}

		Memory->bIsCharging = false;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
	
	if (AAnimalFlying* FlyingAnimal = Cast<AAnimalFlying>(ControlledPawn))
	{
		if (UCharacterMovementComponent* MoveComp = FlyingAnimal->GetCharacterMovement())
		{
			FVector Direction = (Memory->TargetLocation - CurrentLocation).GetSafeNormal();
			MoveComp->Velocity = Direction * ChargeSpeed;
		}
	}
}