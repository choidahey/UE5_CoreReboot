#include "MonsterAI/Animations/AnimNotify/AnimNotify_SpawnActor.h"
#include "AIController.h"
#include "CR4S.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "MonsterAI/Skills/FieldActor.h"
#include "MonsterAI/Skills/SpawnCircleActor.h"


void UAnimNotify_SpawnActor::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!IsValid(MeshComp)
		|| !IsValid(MeshComp->GetWorld())
		|| !IsValid(SpawnFieldActorClass)
		|| !IsValid(Animation)) return;

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
	
	FVector SpawnLocation = OwnerPawn->GetActorLocation();
	FVector FinalOffset = FVector::ZeroVector;

	if (!SpawnLocationOffset.IsNearlyZero())
	{
		FinalOffset = OwnerPawn->GetActorTransform().TransformVector(SpawnLocationOffset);
	}
	
	const FVector TraceStart = SpawnLocation + FVector(0.f, 0.f, TraceHeight);
	const FVector TraceEnd = SpawnLocation - FVector(0.f, 0.f, TraceHeight);

	FHitResult Hit;
	FCollisionQueryParams Params(NAME_None, false, OwnerPawn);
	if (OwnerPawn->GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params))
	{
		SpawnLocation = Hit.Location;
	}

	SpawnLocation += FinalOffset;

	const FRotator SpawnRotation = OwnerPawn->GetActorRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerPawn;
	SpawnParams.Instigator = OwnerPawn->GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* SpawnActor = OwnerPawn->GetWorld()->SpawnActor<AActor>(
		SpawnFieldActorClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);
	if (!IsValid(SpawnActor)) return;
	
	if (AFieldActor* FieldActor = Cast<AFieldActor>(SpawnActor))
	{
		FieldActor->Initialize(Cast<AActor>(OwnerPawn), Target);
	}
	else if (ASpawnCircleActor* SpawnCircle = Cast<ASpawnCircleActor>(SpawnActor))
	{
		SpawnCircle->SpawnInCircle(Cast<AActor>(OwnerPawn));
	}
	else
	{
		CR4S_Log(LogDa, Log, TEXT("[%s] Invaild Blizzard Actor"), *MyHeader);
	}
}
