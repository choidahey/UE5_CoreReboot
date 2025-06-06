#include "MonsterAI/Animations/AnimNotifyState/AnimNotifyState_ThunderRoad.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"

void UAnimNotifyState_ThunderRoad::NotifyBegin(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float TotalDuration)
{
	if (!IsValid(MeshComp->GetWorld())
	|| !IsValid(MeshComp)
	|| !IsValid(SpawnActorClass)
	|| !IsValid(Animation)) return;
	
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
		FVector Dir = TargetActorLocation - OwnerPawn->GetActorLocation();
		InitialDashDirection = Dir.IsNearlyZero() ? FVector::ZeroVector : Dir.GetSafeNormal();
	}
	else
	{
		TargetActorLocation = FVector::ZeroVector;
		InitialDashDirection = FVector::ZeroVector;
	}
	
	const FVector SocketLocation = MeshComp->GetSocketLocation(AttachSocketName);
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Cast<AActor>(OwnerPawn);
	SpawnParams.Instigator = OwnerPawn;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (bIsAttachInSocket)
	{
		SpawnedActor = OwnerPawn->GetWorld()->SpawnActor<AActor>(
		SpawnActorClass,
		SocketLocation,
		InitialActorRotation,
		SpawnParams);
		if (!SpawnedActor) return;

		SpawnedActor->AttachToComponent(MeshComp, FAttachmentTransformRules::KeepWorldTransform, AttachSocketName);
	}
	else
	{
		const FVector SocketWorldLocation = MeshComp->GetSocketLocation(AttachSocketName);
		
		const FVector TraceStart(SocketWorldLocation.X, SocketWorldLocation.Y, SocketWorldLocation.Z + TraceHeight);
		const FVector TraceEnd(SocketWorldLocation.X, SocketWorldLocation.Y, SocketWorldLocation.Z - TraceHeight);

		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(OwnerPawn);

		float GroundZ = SocketWorldLocation.Z;
		if (MeshComp->GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldStatic, Params))
			GroundZ = Hit.Location.Z;
		
		SpawnedActor = MeshComp->GetWorld()->SpawnActor<AActor>(
			SpawnActorClass,
			SocketWorldLocation,
			InitialActorRotation,
			SpawnParams);
		if (!IsValid(SpawnedActor)) return;

		SpawnedActor->AttachToComponent(MeshComp, FAttachmentTransformRules::KeepWorldTransform, AttachSocketName);
		
		FVector NewWorldLoc = FVector(SocketWorldLocation.X, SocketWorldLocation.Y, GroundZ);
		SpawnedActor->SetActorLocation(NewWorldLoc);
	}
}

void UAnimNotifyState_ThunderRoad::NotifyTick(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float FrameDeltaTime)
{
	if (!IsValid(MeshComp)) return;

	APawn* OwnerPawn = Cast<APawn>(MeshComp->GetOwner());
	if (!IsValid(OwnerPawn)) return;

	if (!InitialDashDirection.IsNearlyZero())
	{
		FVector CurrentLoc = OwnerPawn->GetActorLocation();
		float MoveDist = BossMoveSpeed * FrameDeltaTime;
		FVector NewLoc = CurrentLoc + InitialDashDirection * MoveDist;
		OwnerPawn->SetActorLocation(NewLoc);
	}

	if (SpawnedActor)
	{
		FRotator WallRot(0.f, InitialDashDirection.Rotation().Yaw, 0.f);
		SpawnedActor->SetActorRotation(WallRot);

		FVector WallLocation = SpawnedActor->GetActorLocation();
		FRotator WallRotation = SpawnedActor->GetActorRotation();
		FVector BoxExtent(50.f, 200.f, 400.f);

		DrawDebugBox(
			OwnerPawn->GetWorld(),
			WallLocation,
			BoxExtent,
			WallRotation.Quaternion(),
			FColor::Red,
			false,
			0.f,
			0,
			2.f
		);

		FVector ForwardDir = SpawnedActor->GetActorForwardVector();
		float LineLength = 300.f;
		FVector LineEnd = WallLocation + ForwardDir * LineLength;

		DrawDebugLine(
			OwnerPawn->GetWorld(),
			WallLocation,
			LineEnd,
			FColor::Yellow,
			false,
			0.f,
			0,
			2.f
		);
	}
}

void UAnimNotifyState_ThunderRoad::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (SpawnedActor && SpawnedActor->IsValidLowLevel())
		SpawnedActor->Destroy();

	SpawnedActor = nullptr;
	InitialDashDirection = FVector::ZeroVector;
}
