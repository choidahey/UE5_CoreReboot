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
	ConfigureSense(*SightConfig);

	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->HearingRange = 1500.0f;
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
	const bool bIsDetected = Stimulus.WasSuccessfullySensed();
	const FName PlayerTag = TEXT("Player");

	if (!Actor->ActorHasTag(PlayerTag)) return;

	if (bIsDetected)
	{
		OnPlayerDetected.Broadcast(Actor);
	}
	else
	{
		OnPlayerLost.Broadcast(Actor);
	}
}

AActor* UMonsterPerceptionComponent::FindTaggedActorInSense(const TSubclassOf<UAISense> SenseClass, const FName& Tag) const
{
	TArray<AActor*> DetectedActors;
	GetCurrentlyPerceivedActors(SenseClass, DetectedActors);

	for (AActor* Actor : DetectedActors)
	{
		if (Actor && Actor->ActorHasTag(Tag))
		{
			return Actor;
		}
	}

	return nullptr;
}

AActor* UMonsterPerceptionComponent::GetDetectedPlayerBySight() const
{
	return FindTaggedActorInSense(UAISense_Sight::StaticClass(), TEXT("Player"));
}

AActor* UMonsterPerceptionComponent::GetDetectedPlayerByHearing() const
{
	return FindTaggedActorInSense(UAISense_Hearing::StaticClass(), TEXT("Player"));
}

