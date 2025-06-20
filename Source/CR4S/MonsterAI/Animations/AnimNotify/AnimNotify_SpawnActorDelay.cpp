#include "MonsterAI/Animations/AnimNotify/AnimNotify_SpawnActorDelay.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/Skills/IceSpike.h"

void UAnimNotify_SpawnActorDelay::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!IsValid(MeshComp->GetWorld())
		|| !IsValid(MeshComp)
		|| !IsValid(SpawnActorClass)
		|| !IsValid(Animation)) return;
	
	APawn* OwnerPawn = Cast<APawn>(MeshComp->GetOwner());
	if (!OwnerPawn || !SpawnActorClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] Invalid OwnerPawn or SpawnActorClass"), *GetClass()->GetName());
		return;
	}
	
	AAIController* AIC = Cast<AAIController>(OwnerPawn->GetController());
	UBlackboardComponent* BB = AIC->GetBlackboardComponent();
	if (!AIC || !BB) return;

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(FAIKeys::TargetActor));
	Target = Target ? Target : Cast<AActor>(BB->GetValueAsObject(FSeasonBossAIKeys::NearestHouseActor));
	Target = Target ? Target : UGameplayStatics::GetPlayerPawn(MeshComp->GetWorld(), 0);

	FVector CenterLoc = Target->GetActorLocation();
	float HalfHeight = 0.f;
	if (ACharacter* Char = Cast<ACharacter>(Target))
	{
		if (UCapsuleComponent* Cap = Char->GetCapsuleComponent())
		{
			HalfHeight = Cap->GetScaledCapsuleHalfHeight();
		}
	}
	
	FVector SpawnLocation = CenterLoc - FVector(0.f, 0.f, HalfHeight);
		
	const float OffsetZ = 200.f;
	FHitResult Hit;
	FVector TraceStart = SpawnLocation + FVector(0, 0, OffsetZ);
	FVector TraceEnd = TraceStart - FVector(0, 0, OffsetZ);

	FCollisionQueryParams CollisionParam;
	CollisionParam.AddIgnoredActor(OwnerPawn);

	if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd,ECC_Visibility, CollisionParam))
	{
		float OffsetLoc = SpawnLocation.Z - Hit.Location.Z;
		if (!FMath::IsNearlyZero(OffsetLoc, 1.0f))
		{
			SpawnLocation.Z -= OffsetLoc;
		}
	}

	const FVector SavedLocation = SpawnLocation;
	FTimerDelegate Del;
	Del.BindLambda([OwnerPawn, this, SavedLocation]()
	{
		if (!IsValid(OwnerPawn) || !SpawnActorClass) return;
		
		FActorSpawnParameters Params;
		Params.Owner = OwnerPawn;
		Params.Instigator = OwnerPawn;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AActor* NewActor = OwnerPawn->GetWorld()->SpawnActor<AActor>(
			SpawnActorClass,
			SavedLocation,
			FRotator::ZeroRotator,
			Params);
		
		if (AIceSpike* Spike = Cast<AIceSpike>(NewActor))
			Spike->Launch();
	});


	MeshComp->GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
	MeshComp->GetWorld()->GetTimerManager().SetTimer(
		SpawnTimerHandle,
		Del,
		SpawnDelayTime,
		false
		);
}
