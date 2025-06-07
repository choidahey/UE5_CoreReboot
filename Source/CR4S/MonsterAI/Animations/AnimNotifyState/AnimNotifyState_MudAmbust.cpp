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
	if (!IsValid(MeshComp->GetWorld())
		|| !IsValid(MeshComp)
		|| !IsValid(MudFieldClass)
		|| !IsValid(Animation)) return;

	ElapsedTime = 0.f;
	Duration = TotalDuration;

	APawn* OwnerPawn = Cast<APawn>(MeshComp->GetOwner());
	if (!OwnerPawn) return;
	
	AAIController* AIC = Cast<AAIController>(OwnerPawn->GetController());
	UBlackboardComponent* BB = AIC->GetBlackboardComponent();
	if (!AIC || !BB) return;
    
	AActor* Target = Cast<AActor>(BB->GetValueAsObject(TEXT("TargetActor")));
	Target = Target ? Target : Cast<AActor>(BB->GetValueAsObject(TEXT("NearestHouseActor")));
	Target = Target ? Target : UGameplayStatics::GetPlayerPawn(MeshComp->GetWorld(), 0);

	TargetActor = Target;
	StartLocation = OwnerPawn->GetActorLocation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = MeshComp->GetOwner();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	FVector SpawnLoc = StartLocation + FVector(0,0, TraceStartZ);
	FRotator SpawnRot = FRotator::ZeroRotator;
	
	AActor* Field = MeshComp->GetWorld()->SpawnActor<AActor>(MudFieldClass, SpawnLoc, SpawnRot, SpawnParams);

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
	if (!SpawnedMudField || !MeshComp) return;

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
	// DrawDebugLine(MeshComp->GetWorld(), TraceStart, TraceEnd, FColor::Blue, false, 0.1f);

	if (bHit)
	{
		SpawnedMudField->SetActorLocation(Hit.ImpactPoint);
	}
	else
	{
		FVector Fallback = BossLoc;
		Fallback.Z = BossLoc.Z - TraceDownDistance * 0.5f;
		SpawnedMudField->SetActorLocation(Fallback);
	}
	
	ElapsedTime += FrameDeltaTime;
	float Alpha = FMath::Clamp(ElapsedTime / Duration, 0.f, 1.f);
	
	TargetLocation = IsValid(TargetActor) ? TargetActor->GetActorLocation() : StartLocation;
	
	FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation, Alpha);
	MeshComp->GetOwner()->SetActorLocation(NewLocation);
}

void UAnimNotifyState_MudAmbust::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AActor* OwnerActor = MeshComp->GetOwner();

	if (SpawnedMudField)
	{
		SpawnedMudField->Destroy();
		SpawnedMudField = nullptr;
	}

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
