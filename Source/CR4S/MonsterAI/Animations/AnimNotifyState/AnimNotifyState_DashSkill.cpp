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
			AActor* NewSpawnedActor = MeshComp->GetWorld()->SpawnActor<AActor>(
				SpawnActorClass,
				SocketLocation,
				InitialActorRotation,
				SpawnParams);
            	
			if (NewSpawnedActor)
			{
				NewSpawnedActor->AttachToComponent(MeshComp, FAttachmentTransformRules::KeepWorldTransform, ActorAttachSocketName);
				SpawnedActor = NewSpawnedActor;
			}
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

			AActor* NewSpawnedActor = MeshComp->GetWorld()->SpawnActor<AActor>(
				SpawnActorClass,
				SocketLocation,
				InitialActorRotation,
				SpawnParams);

			if (NewSpawnedActor)
			{
				NewSpawnedActor->AttachToComponent(MeshComp, FAttachmentTransformRules::KeepWorldTransform, ActorAttachSocketName);
				NewSpawnedActor->SetActorLocation(FVector(SocketLocation.X, SocketLocation.Y, GroundZ));
				SpawnedActor = NewSpawnedActor;
			}
		}
    }
	
	if (bUseNiagaraEffect && IsValid(NiagaraEffect))
	{
		FName EffSocket = NiagaraAttachSocketName.IsNone() ? NAME_None : NiagaraAttachSocketName;
		UNiagaraComponent* NewNiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
			NiagaraEffect,
			MeshComp,
			EffSocket,
			NiagaraLocationOffset,
			NiagaraRotationOffset,
			EAttachLocation::KeepRelativeOffset,
			true);

		if (NewNiagaraComp)
		{
			NewNiagaraComp->SetRelativeScale3D(NiagaraScale);
			SpawnedNiagaraComp = NewNiagaraComp;
		}
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
	CleanupSpawnedActors();
	InitialDashDirection = FVector::ZeroVector;
}

void UAnimNotifyState_DashSkill::BeginDestroy()
{
	// 객체가 파괴될 때 스폰된 Actor들을 정리
	CleanupSpawnedActors();
	Super::BeginDestroy();
}

void UAnimNotifyState_DashSkill::CleanupSpawnedActors()
{
	// 스폰된 Actor 정리
	if (AActor* Actor = SpawnedActor.Get())
	{
		if (IsValid(Actor))
		{
			Actor->Destroy();
		}
		SpawnedActor = nullptr;
	}

	// 스폰된 Niagara 컴포넌트 정리
	if (UNiagaraComponent* NiagaraComp = SpawnedNiagaraComp.Get())
	{
		if (IsValid(NiagaraComp))
		{
			NiagaraComp->DestroyComponent();
		}
		SpawnedNiagaraComp = nullptr;
	}
}
