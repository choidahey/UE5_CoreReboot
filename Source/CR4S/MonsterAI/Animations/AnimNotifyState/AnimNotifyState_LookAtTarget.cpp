#include "MonsterAI/Animations/AnimNotifyState/AnimNotifyState_LookAtTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"

void UAnimNotifyState_LookAtTarget::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (!MeshComp) return;

	CachedOwner = MeshComp->GetOwner();
	if (ACharacter* Character = Cast<ACharacter>(CachedOwner.Get()))
	{
		CachedController = Character->GetController();
	}

	if (CachedOwner.IsValid())
	{
		InitialActorRotation = CachedOwner->GetActorRotation();
	}
	if (CachedController.IsValid())
	{
		InitialControlRotation = CachedController->GetControlRotation();
	}
}

void UAnimNotifyState_LookAtTarget::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	if (!CachedOwner.IsValid()) return;

	AAIController* AIC = Cast<AAIController>(CachedController.Get());
	if (!AIC) return;

	UBlackboardComponent* BB = AIC->GetBlackboardComponent();
	if (!BB) return;

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(TargetBlackboardKey));
	if (!IsValid(Target)) return;

	const FVector MyLocation = CachedOwner->GetActorLocation();
	const FVector TargetLocation = Target->GetActorLocation();
	FRotator TargetRot = UKismetMathLibrary::FindLookAtRotation(MyLocation, TargetLocation);

	if (!bRotateHeadPitch)
	{
		TargetRot.Pitch = 0.f;
	}
	else
	{
		TargetRot.Pitch = FMath::Clamp(TargetRot.Pitch, -MaxPitchAngle, MaxPitchAngle);
	}
	TargetRot.Roll = 0.f;

	if (bRotateBodyYaw)
	{
		FRotator Current = CachedOwner->GetActorRotation();
		TargetRot.Pitch = 0.f;
		FRotator NewRot = FMath::RInterpTo(Current, TargetRot, FrameDeltaTime, InterpSpeed);
		CachedOwner->SetActorRotation(NewRot);
	}
	else if (CachedController.IsValid())
	{
		FRotator Current = CachedController->GetControlRotation();
		FRotator NewRot = FMath::RInterpTo(Current, TargetRot, FrameDeltaTime, InterpSpeed);
		CachedController->SetControlRotation(NewRot);
	}
}

void UAnimNotifyState_LookAtTarget::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!bResetRotationOnEnd) return;

	if (bRotateBodyYaw && CachedOwner.IsValid())
	{
		CachedOwner->SetActorRotation(InitialActorRotation);
	}

	if (!bRotateBodyYaw && CachedController.IsValid())
	{
		CachedController->SetControlRotation(InitialControlRotation);
	}
}
