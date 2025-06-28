#include "MonsterAI/Animations/AnimNotifyState/AnimNotifyState_MudAmbust.h"
#include "MonsterAI/Skills/MudField.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"


void UAnimNotifyState_MudAmbust::NotifyBegin(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float TotalDuration)
{
	if (!IsValid(MeshComp)
		|| !IsValid(MeshComp->GetWorld())
		|| !IsValid(MudFieldClass)
		|| !IsValid(Animation)) return;
	if (MeshComp->GetWorld()->WorldType != EWorldType::Game && MeshComp->GetWorld()->WorldType != EWorldType::PIE) return;

	ElapsedTime = 0.f;
	Duration = TotalDuration;

	APawn* OwnerPawn = Cast<APawn>(MeshComp->GetOwner());
	if (!IsValid(OwnerPawn)) return;
	
	AAIController* AIC = Cast<AAIController>(OwnerPawn->GetController());
	if (!IsValid(AIC)) return;
	
	UBlackboardComponent* BB = AIC->GetBlackboardComponent();
	if (!IsValid(BB)) return;
    
	AActor* Target = Cast<AActor>(BB->GetValueAsObject(FAIKeys::TargetActor));
	if (!IsValid(Target))
		Target = Cast<AActor>(BB->GetValueAsObject(FSeasonBossAIKeys::NearestHouseActor));
	if (!IsValid(Target))
		Target = UGameplayStatics::GetPlayerPawn(MeshComp->GetWorld(), 0);
	if (!IsValid(Target)) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = MeshComp->GetOwner();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	StartLocation = OwnerPawn->GetActorLocation();
	FVector SpawnLoc = StartLocation + FVector(0,0, TraceStartZ);
	FRotator SpawnRot = FRotator::ZeroRotator;
	
	AActor* Field = MeshComp->GetWorld()->SpawnActor<AActor>(MudFieldClass, SpawnLoc, SpawnRot, SpawnParams);

	TargetActor = Target;
	SpawnedMudField = Field;
	
	if (ACharacter* OwnerCharacter = Cast<ACharacter>(OwnerPawn))
	{
		OwnerCharacter->GetCharacterMovement()->GravityScale = 0.f;
		OwnerCharacter->GetCharacterMovement()->DisableMovement();
		OwnerCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void UAnimNotifyState_MudAmbust::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime)
{
	if (!IsValid(MeshComp)) return;
	if (MeshComp->GetWorld()->WorldType != EWorldType::Game && MeshComp->GetWorld()->WorldType != EWorldType::PIE) return;
	if (!SpawnedMudField.IsValid() || !TargetActor.IsValid()) return;

	AActor* FieldPtr = SpawnedMudField.Get();
	AActor* TargetPtr = TargetActor.Get();
	
	FVector BossLoc = MeshComp->GetOwner()->GetActorLocation();
	FVector TraceStart = BossLoc + FVector(0,0,TraceStartZ);
	FVector TraceEnd = BossLoc - FVector(0,0,TraceDownDistance);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(MeshComp->GetOwner());
	
	bool bHit = MeshComp->GetWorld()->LineTraceSingleByChannel(
		Hit,
		TraceStart,
		TraceEnd,
		ECC_Visibility,
		Params
	);

	if (bHit)
	{
		FieldPtr->SetActorLocation(Hit.ImpactPoint);
	}
	else
	{
		FVector Fallback = BossLoc;
		Fallback.Z = BossLoc.Z - TraceDownDistance * 0.5f;
		FieldPtr->SetActorLocation(Fallback);
	}
	
	ElapsedTime += FrameDeltaTime;
	float Alpha = FMath::Clamp(ElapsedTime / Duration, 0.f, 1.f);
	
	TargetLocation = IsValid(TargetPtr) ? TargetActor->GetActorLocation() : StartLocation;
	
	FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation, Alpha);
	MeshComp->GetOwner()->SetActorLocation(NewLocation);
}

void UAnimNotifyState_MudAmbust::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!IsValid(MeshComp)) return;
	if (MeshComp->GetWorld()->WorldType != EWorldType::Game && MeshComp->GetWorld()->WorldType != EWorldType::PIE) return;

	MeshComp->GetWorld()->GetTimerManager().ClearTimer(RiseTimerHandle);
	
	AActor* OwnerActor = MeshComp->GetOwner();
	if (!IsValid(OwnerActor)) return;

	if (SpawnedMudField.IsValid())
	{
		SpawnedMudField->Destroy();
		SpawnedMudField.Reset();
	}

	TargetActor.Reset();

	const FVector FinalLocation = TargetLocation;
	TWeakObjectPtr<AActor> WeakOwner = OwnerActor;

	FTimerDelegate LocationDel;
	LocationDel.BindLambda([WeakOwner, FinalLocation]()
	{
		if (AActor* P = WeakOwner.Get())
		{
			P->SetActorLocation(FinalLocation);
		}
	});
	
	if (ACharacter* OwnerCharacter = Cast<ACharacter>(OwnerActor))
	{
		OwnerCharacter->GetCharacterMovement()->GravityScale = 1.f;
		OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		OwnerCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	OwnerActor->GetWorld()->GetTimerManager().SetTimer(
		RiseTimerHandle,
		LocationDel,
	   1.0f,
	   false
	   );
}
