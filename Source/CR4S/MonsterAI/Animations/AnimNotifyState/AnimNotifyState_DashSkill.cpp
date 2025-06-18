#include "MonsterAI/Animations/AnimNotifyState/AnimNotifyState_DashSkill.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "NiagaraComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "NiagaraFunctionLibrary.h"


void UAnimNotifyState_DashSkill::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (!IsValid(MeshComp->GetWorld()) || !IsValid(MeshComp) || !IsValid(Animation)) return;
	
	APawn* OwnerPawn = Cast<APawn>(MeshComp->GetOwner());
	if (!IsValid(OwnerPawn)) return;
	
	AAIController* AIC = Cast<AAIController>(OwnerPawn->GetController());
	if (!IsValid(AIC)) return;

	UBrainComponent* BrainComp = AIC->GetBrainComponent();
	if (!IsValid(BrainComp)) return;
	
	UBlackboardComponent* BB = AIC->GetBlackboardComponent();
	if (!IsValid(BB)) return;
    
	AActor* Target = Cast<AActor>(BB->GetValueAsObject(FAIKeys::TargetActor));
	Target = Target ? Target : Cast<AActor>(BB->GetValueAsObject(FSeasonBossAIKeys::NearestHouseActor));
	Target = Target ? Target : UGameplayStatics::GetPlayerPawn(MeshComp->GetWorld(), 0);
	
	if (IsValid(Target))
	{
		TargetActorLocation = Target->GetActorLocation();
		FVector Dir = (TargetActorLocation - OwnerPawn->GetActorLocation()).GetSafeNormal();
		InitialDashDirection = bOppositeDirection ? -Dir : Dir;
	}
	else
	{
		TargetActorLocation = FVector::ZeroVector;
		InitialDashDirection = FVector::ZeroVector;
	}
	
	StartingLocation = OwnerPawn->GetActorLocation();
	const FVector SocketLocation = MeshComp->GetSocketLocation(ActorAttachSocketName);
	// if (DashDistance > 0.0f && TotalDuration > 0.0f)
	// {
	// 	BossMoveSpeed = DashDistance / TotalDuration;
	// }
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Cast<AActor>(OwnerPawn);
	SpawnParams.Instigator = OwnerPawn;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (bUseActorSpawn && IsValid(SpawnActorClass))
    {
		if (bIsAttachInSocket)
		{
			SpawnedActor = MeshComp->GetWorld()->SpawnActor<AActor>(
				SpawnActorClass,
				SocketLocation,
				InitialActorRotation,
				SpawnParams);
            	
			SpawnedActor->AttachToComponent(MeshComp, FAttachmentTransformRules::KeepWorldTransform, ActorAttachSocketName);
		}
		else
		{
			FVector TraceStart = SocketLocation + FVector(0, 0, TraceHeight);
			FVector TraceEnd   = SocketLocation - FVector(0, 0, TraceHeight);
			FHitResult Hit;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(OwnerPawn);

			float GroundZ = SocketLocation.Z;
			if (MeshComp->GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldStatic, Params))
			{
				GroundZ = Hit.Location.Z;
			}

			SpawnedActor = MeshComp->GetWorld()->SpawnActor<AActor>(
				SpawnActorClass,
				SocketLocation,
				InitialActorRotation,
				SpawnParams);

			SpawnedActor->AttachToComponent(MeshComp, FAttachmentTransformRules::KeepWorldTransform, ActorAttachSocketName);
			SpawnedActor->SetActorLocation(FVector(SocketLocation.X, SocketLocation.Y, GroundZ));
		}
    }
	
	if (bUseNiagaraEffect && IsValid(NiagaraEffect))
	{
		FName EffSocket = NiagaraAttachSocketName.IsNone() ? NAME_None : NiagaraAttachSocketName;
		SpawnedNiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
			NiagaraEffect,
			MeshComp,
			EffSocket,
			NiagaraLocationOffset,
			NiagaraRotationOffset,
			EAttachLocation::KeepRelativeOffset,
			true);

		SpawnedNiagaraComp->SetRelativeScale3D(NiagaraScale);
	}
}

void UAnimNotifyState_DashSkill::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	if (!IsValid(MeshComp)) return;

	APawn* OwnerPawn = Cast<APawn>(MeshComp->GetOwner());
	if (!IsValid(OwnerPawn) || InitialDashDirection.IsNearlyZero()) return;

	// float EffectiveSpeed = (DashDistance > 0.f && Animation) ? (DashDistance / Animation->GetPlayLength()) : BossMoveSpeed;
	float MoveStep = BossMoveSpeed * FrameDeltaTime;
	const FVector CurrentLoc = OwnerPawn->GetActorLocation();
	FVector DesiredLoc = CurrentLoc + InitialDashDirection * MoveStep;
	
	OwnerPawn->SetActorLocation(DesiredLoc, false, nullptr, ETeleportType::TeleportPhysics);
}

void UAnimNotifyState_DashSkill::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (SpawnedActor && SpawnedActor->IsValidLowLevel())
		SpawnedActor->Destroy();

	SpawnedActor = nullptr;
	InitialDashDirection = FVector::ZeroVector;

	if (SpawnedNiagaraComp && SpawnedNiagaraComp->IsValidLowLevel())
	{
		SpawnedNiagaraComp->DestroyComponent();
		SpawnedNiagaraComp = nullptr;
	}
}
