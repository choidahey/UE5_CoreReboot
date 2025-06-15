#include "MonsterAI/Animations/AnimNotify/AnimNotify_SpawnActor.h"
#include "AIController.h"
#include "CR4S.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "MonsterAI/Skills/AcidWaveActor.h"
#include "MonsterAI/Skills/FieldActor.h"
#include "MonsterAI/Skills/SpawnCircleActor.h"


void UAnimNotify_SpawnActor::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!IsValid(MeshComp->GetWorld())
		|| !IsValid(MeshComp)
		|| !IsValid(SpawnFieldActorClass)
		|| !IsValid(Animation)) return;

	APawn* OwnerPawn = Cast<APawn>(MeshComp->GetOwner());
	if (!OwnerPawn) return;

	AAIController* AIC = Cast<AAIController>(OwnerPawn->GetController());
	UBlackboardComponent* BB = AIC->GetBlackboardComponent();
	if (!AIC || !BB) return;
    
	AActor* Target = Cast<AActor>(BB->GetValueAsObject(FAIKeys::TargetActor));
	Target = Target ? Target : Cast<AActor>(BB->GetValueAsObject(FSeasonBossAIKeys::NearestHouseActor));
	Target = Target ? Target : UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	
	FVector SpawnLocation = OwnerPawn->GetActorLocation();
	const FVector TraceStart = SpawnLocation + FVector(0.f, 0.f, TraceHeight);
	const FVector TraceEnd   = SpawnLocation - FVector(0.f, 0.f, TraceHeight);

	FHitResult Hit;
	FCollisionQueryParams Params(NAME_None, false, OwnerPawn);
	if (OwnerPawn->GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params))
	{
		SpawnLocation = Hit.Location;
	}
	
	if (!SpawnLocationOffset.IsNearlyZero())
	{
		SpawnLocation += SpawnLocationOffset;
	}

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
	if (!SpawnActor) return;

	// SpawnActor->AddActorLocalOffset(SpawnLocationOffset);
	
	if (AFieldActor* FieldActor = Cast<AFieldActor>(SpawnActor))
	{
		FieldActor->Initialize(Cast<AActor>(OwnerPawn), Target);
	}
	else if (AAcidWaveActor* WaveActor = Cast<AAcidWaveActor>(SpawnActor))
	{
		WaveActor->Initialize(Cast<AActor>(OwnerPawn), Target);
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
