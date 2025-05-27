#include "MonsterAI/Animations/AnimNotify/AnimNotify_IceSpikeAttack.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "MonsterAI/Components/MonsterStateComponent.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "MonsterAI/Skills/IceSpike.h"

void UAnimNotify_IceSpikeAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp || !SpawnActorClass) return;
	
	APawn* OwnerPawn = Cast<APawn>(MeshComp->GetOwner());
	if (!OwnerPawn || !SpawnActorClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] Invalid OwnerPawn or SpawnActorClass"), *MyHeader);
		return;
	}

	AActor* TargetActor = nullptr;
	AAIController* AIC = Cast<AAIController>(OwnerPawn->GetController());
	UBlackboardComponent* BlackboardComp = AIC->GetBlackboardComponent();
	if (!AIC || !BlackboardComp)
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] Invalid AIC or Blackboard Component"), *MyHeader);
		return;
	}

	TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(FAIKeys::TargetActor));
	if (!TargetActor)
	{
		TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(FSeasonBossAIKeys::NearestHouseActor));
	}

	FVector CenterLoc = TargetActor->GetActorLocation();
	float HalfHeight = 0.f;
	if (ACharacter* Char = Cast<ACharacter>(TargetActor))
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

	if (GetWorld()->LineTraceSingleByChannel(
			Hit, TraceStart, TraceEnd,
			ECC_Visibility, CollisionParam))
	{
		float OffsetLoc = SpawnLocation.Z - Hit.Location.Z;
		if (!FMath::IsNearlyZero(OffsetLoc, 1.0f))
		{
			SpawnLocation.Z -= OffsetLoc;
		}
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerPawn;
	SpawnParams.Instigator = OwnerPawn;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* SpawnActor = OwnerPawn->GetWorld()->SpawnActor<AActor>(
		SpawnActorClass,
		SpawnLocation,
		FRotator::ZeroRotator,
		SpawnParams
	);
	if (!SpawnActor) return;

	if (AIceSpike* SpikeActor = Cast<AIceSpike>(SpawnActor))
	{
		if (SpikeActor)
			SpikeActor->Launch();
	}
}
