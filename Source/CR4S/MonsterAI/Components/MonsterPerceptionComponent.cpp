#include "CR4S/MonsterAI/Components/MonsterPerceptionComponent.h"
#include "CR4S/MonsterAI/Data/MonsterAttributeRow.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

UMonsterPerceptionComponent::UMonsterPerceptionComponent()
	: MyHeader(TEXT("MonsterPercComp"))
{
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));

	SetDominantSense(UAISense_Sight::StaticClass());
}

void UMonsterPerceptionComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UMonsterPerceptionComponent::ApplySightConfigFromAttribute(const FMonsterAttributeRow& Data)
{
	SightConfig->SightRadius = Data.SightRadius;
	SightConfig->LoseSightRadius = Data.LoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = Data.PeripheralVisionAngleDegrees;

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	ConfigureSense(*SightConfig);

	HearingConfig->HearingRange = Data.HearingRange;

	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;

	ConfigureSense(*HearingConfig);

	OnTargetPerceptionUpdated.AddDynamic(this, &UMonsterPerceptionComponent::OnUpdatePerception);
	RequestStimuliListenerUpdate();

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

