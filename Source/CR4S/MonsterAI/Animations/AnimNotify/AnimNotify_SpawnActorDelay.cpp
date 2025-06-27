#include "MonsterAI/Animations/AnimNotify/AnimNotify_SpawnActorDelay.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "AIController.h"
#include "CR4S.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/Skills/FieldActor.h"
#include "MonsterAI/Skills/IceSpike.h"

void UAnimNotify_SpawnActorDelay::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!IsValid(MeshComp)
		|| !IsValid(MeshComp->GetWorld())
		|| !IsValid(SpawnActorClass)
		|| !IsValid(Animation)) return;
	
	APawn* OwnerPawn = Cast<APawn>(MeshComp->GetOwner());
	if (!IsValid(OwnerPawn) || !IsValid(SpawnActorClass))
	{
		CR4S_Log(LogDa, Warning, TEXT("[%s] Invalid OwnerPawn or SpawnActorClass"), *GetClass()->GetName());
		return;
	}
	
	AAIController* AIC = Cast<AAIController>(OwnerPawn->GetController());
	if (!IsValid(AIC)) return;
	
	UBlackboardComponent* BB = AIC->GetBlackboardComponent();
	if (!IsValid(BB)) return;

	AActor* TargetPtr = Cast<AActor>(BB->GetValueAsObject(FAIKeys::TargetActor));
	if (!IsValid(TargetPtr))
		TargetPtr = Cast<AActor>(BB->GetValueAsObject(FSeasonBossAIKeys::NearestHouseActor));
	if (!IsValid(TargetPtr))
		TargetPtr = UGameplayStatics::GetPlayerPawn(MeshComp->GetWorld(), 0);
	if (!IsValid(TargetPtr))
		return;

	FVector CenterLoc = TargetPtr->GetActorLocation();
	float HalfHeight = 0.f;
	if (ACharacter* Char = Cast<ACharacter>(TargetPtr))
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

	if (MeshComp->GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd,ECC_Visibility, CollisionParam))
	{
		float OffsetLoc = SpawnLocation.Z - Hit.Location.Z;
		if (!FMath::IsNearlyZero(OffsetLoc, 1.0f))
		{
			SpawnLocation.Z -= OffsetLoc;
		}
	}

	APawn* PawnPtr = OwnerPawn;
	AActor* TargetActorPtr = TargetPtr;
	TSubclassOf<AActor> SpawnClass = SpawnActorClass;
	FVector SpawnPosition = SpawnLocation;
	float DelayTime = SpawnDelayTime;
	
	FTimerHandle TempHandle;
	FTimerDelegate CreateDelegate = FTimerDelegate::CreateLambda(
		[PawnPtr, SpawnClass, TargetActorPtr, SpawnPosition]()
		{
			if (!IsValid(PawnPtr) || SpawnClass == nullptr)
				return;

			FActorSpawnParameters ParamsSP;
			ParamsSP.Owner = PawnPtr;
			ParamsSP.Instigator= PawnPtr;
			ParamsSP.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			AActor* NewActor = PawnPtr->GetWorld()->SpawnActor<AActor>(
				SpawnClass,
				SpawnPosition,
				FRotator::ZeroRotator,
				ParamsSP
			);
			if (!IsValid(NewActor))
				return;
			
			if (AFieldActor* Field = Cast<AFieldActor>(NewActor))
			{
				Field->Initialize(PawnPtr, TargetActorPtr);
			}
		}
	);

	MeshComp->GetWorld()->GetTimerManager().SetTimer(TempHandle, CreateDelegate, DelayTime, false);
}
