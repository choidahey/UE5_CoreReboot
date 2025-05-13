#include "CR4S/MonsterAI/Components/MonsterPerceptionComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

UMonsterPerceptionComponent::UMonsterPerceptionComponent()
	: MyHeader(TEXT("MonsterPercComp"))
{
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 2000.0f;
	SightConfig->LoseSightRadius = 3000.0f;
	SightConfig->PeripheralVisionAngleDegrees = 90.0f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	ConfigureSense(*SightConfig);

	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->HearingRange = 1500.0f;
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = false;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = false;
	ConfigureSense(*HearingConfig);

	SetDominantSense(UAISense_Sight::StaticClass());
}

void UMonsterPerceptionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OnTargetPerceptionUpdated.AddDynamic(this, &UMonsterPerceptionComponent::OnUpdatePerception);

	// NOTICE :: Test Log
	UE_LOG(LogTemp, Log, TEXT("[%s] Sight Radius = %.0f, LoseRadius = %.0f, SightAngle = %.0f"),
		*MyHeader,
		SightConfig->SightRadius,
		SightConfig->LoseSightRadius,
		SightConfig->PeripheralVisionAngleDegrees);

	UE_LOG(LogTemp, Log, TEXT("[%s] Hearing Range = %.0f"),
		*MyHeader,
		HearingConfig->HearingRange);

    UAISense_Hearing::ReportNoiseEvent(
       GetWorld(),
       GetOwner()->GetActorLocation(),
       1.0f,
       GetOwner()
    );
}

void UMonsterPerceptionComponent::OnUpdatePerception(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		OnActorDetected.Broadcast(Actor);
	}
	else
	{
		OnActorLost.Broadcast(Actor);
	}
}

TArray<AActor*> UMonsterPerceptionComponent::GetSightInActors() const
{
	TArray<AActor*> DetecteActors;
	GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), DetecteActors);

	return DetecteActors;
}

TArray<AActor*> UMonsterPerceptionComponent::GetHearingInAcotrs() const
{
	TArray<AActor*> DetecteActors;
	GetCurrentlyPerceivedActors(UAISense_Hearing::StaticClass(), DetecteActors);

	return DetecteActors;
}
