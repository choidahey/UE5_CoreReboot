#include "MonsterAI/Animations/AnimNotifyState/AnimNotifyState_TickMoveToTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

void UAnimNotifyState_TickMoveToTarget::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	ElapsedTime = 0.f;
	ActualMoveDuration = TotalDuration;

	if (ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner()))
	{
		StartLocation = Character->GetActorLocation();
        UE_LOG(LogTemp, Warning, TEXT("[TickMoveToTarget::Begin] StartLocation: %s"), *StartLocation.ToString());

		if (AAIController* AI = Cast<AAIController>(Character->GetController()))
		{
			if (UBlackboardComponent* BB = AI->GetBlackboardComponent())
			{
                FVector RawTargetLocation = FVector::ZeroVector;
				if (bUseTargetActor)
				{
					if (AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(TargetActorBlackboardKey)))
					{
						RawTargetLocation = TargetActor->GetActorLocation();
					}
				}
				else
				{
					RawTargetLocation = BB->GetValueAsVector(BlackboardKeyName);
				}

				const FVector Direction = (RawTargetLocation - StartLocation).GetSafeNormal();
				const FVector OffsetLocation = RawTargetLocation + Direction * FrontOffset;

				if (bUseFixedTravelDistance)
				{
					const float MaxDist = FVector::Dist(OffsetLocation, StartLocation);
					const float ClampedDistance = FMath::Clamp(DesiredTravelDistance, 0.f, MaxDist);
					TargetLocation = StartLocation + Direction * ClampedDistance;
				}
				else
				{
					TargetLocation = OffsetLocation;
				}
			}
		}
	}
}

void UAnimNotifyState_TickMoveToTarget::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	ElapsedTime += FrameDeltaTime;

    if (ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner()))
    {
        float Alpha = FMath::Clamp(ElapsedTime / ActualMoveDuration, 0.f, 1.f);
        FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation, Alpha);

        if (bUseJumpCurve)
        {
            float HeightOffset = FMath::Sin(Alpha * PI) * JumpHeight;
            NewLocation.Z += HeightOffset;
        }

        FVector TraceStart = NewLocation + FVector(0, 0, 100.f);
        FVector TraceEnd = NewLocation - FVector(0, 0, 1000.f);

        FHitResult Hit;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(Character);

        if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params))
        {
            float CapsuleHalfHeight = Character->GetSimpleCollisionHalfHeight();
            NewLocation.Z = Hit.ImpactPoint.Z + CapsuleHalfHeight;
        }

        Character->SetActorLocation(NewLocation, true);
    }
}

void UAnimNotifyState_TickMoveToTarget::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner()))
    {
        FVector FinalLocation = Character->GetActorLocation();
        FVector TotalDelta = FinalLocation - StartLocation;
        UE_LOG(LogTemp, Warning, TEXT("[TickMoveToTarget::End] FinalLocation: %s | MovedDelta: %s"), *FinalLocation.ToString(), *TotalDelta.ToString());
    }
}
