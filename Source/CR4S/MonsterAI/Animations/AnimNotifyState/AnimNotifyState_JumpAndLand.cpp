#include "MonsterAI/Animations/AnimNotifyState/AnimNotifyState_JumpAndLand.h"
#include "MonsterAI/Region/RegionBossMonster.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"

void UAnimNotifyState_JumpAndLand::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner());
	if (!IsValid(Character)) return;

	AController* AIController = Cast<AAIController>(Character->GetController());
	UBlackboardComponent* BB = AIController ? AIController->FindComponentByClass<UBlackboardComponent>() : nullptr;
	if (!IsValid(BB)) return;

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(TargetActorKeyName));
	if (!IsValid(TargetActor)) return;

	FVector StartLocation = Character->GetActorLocation();
	FVector TargetLocation = TargetActor->GetActorLocation();
	FVector Velocity = TargetActor->GetVelocity();
	FVector PredictedLocation = TargetLocation + Velocity * PredictTime;
	PredictedLocation.Z = TargetLocation.Z;

	FVector FlatDir = (PredictedLocation - StartLocation);
	float DistanceXY = FlatDir.Size2D();
	FlatDir.Z = 0.f;
	FlatDir.Normalize();

	float GravityZ = FMath::Abs(Character->GetWorld()->GetGravityZ());
	float ArcHeight = FMath::Clamp(DistanceXY * ArcHeightRatio, 300.f, 900.f);
	float VerticalSpeed = FMath::Sqrt(2.f * GravityZ * ArcHeight);

	FVector LaunchVelocity = FlatDir * JumpSpeed;
	LaunchVelocity.Z = VerticalSpeed;

	Character->LaunchCharacter(LaunchVelocity, true, true);

	FRotator LookAt = (PredictedLocation - StartLocation).Rotation();
	LookAt.Pitch = 0.f;
	Character->SetActorRotation(LookAt);

#if WITH_EDITOR
	DrawDebugLine(Character->GetWorld(), StartLocation, TargetLocation, FColor::Purple, false, 2.f, 0, 4.f);
	DrawDebugDirectionalArrow(Character->GetWorld(), StartLocation, StartLocation + LaunchVelocity, 100.f, FColor::Magenta, false, 2.f, 0, 3.f);
#endif
}

void UAnimNotifyState_JumpAndLand::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner());
	if (!IsValid(Character)) return;

	FVector CurrentVelocity = Character->GetVelocity();
	CurrentVelocity.Z = FallZVelocity;
	Character->LaunchCharacter(CurrentVelocity, true, true);
}
