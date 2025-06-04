#include "Game/System/EnvironmentalModifierVolume.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Character/Components/EnvironmentalStatusComponent.h"
#include "CR4S.h"

AEnvironmentalModifierVolume::AEnvironmentalModifierVolume()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneComponent;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->SetupAttachment(RootComponent);

	CapsuleComponent->InitCapsuleSize(Radius, Height);
	CapsuleComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	CapsuleComponent->SetGenerateOverlapEvents(true);

	CapsuleComponent->SetHiddenInGame(false);

	CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnvironmentalModifierVolume::OnBeginOverlap);
	CapsuleComponent->OnComponentEndOverlap.AddDynamic(this, &AEnvironmentalModifierVolume::OnEndOverlap);
}

void AEnvironmentalModifierVolume::BeginPlay()
{
	Super::BeginPlay();
}

void AEnvironmentalModifierVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnvironmentalModifierVolume::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this) return;

	UE_LOG(LogEnvironment, Warning, TEXT("EnvironmentalModifierVolume BeginOverlap"));

	UEnvironmentalStatusComponent* EnvComp = OtherActor->FindComponentByClass<UEnvironmentalStatusComponent>();
	if (EnvComp && !OverlappingActors.Contains(EnvComp))
	{
		EnvComp->ModifyTemperature(TemperatureDelta, ChangeSpeed);
		EnvComp->ModifyHumidity(HumidityDelta, ChangeSpeed); 

		OverlappingActors.Add(EnvComp, 0.0f); 
	}
}

void AEnvironmentalModifierVolume::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor || OtherActor == this) return;

	UE_LOG(LogEnvironment, Warning, TEXT("EnvironmentalModifierVolume EndOverlap"));

	UEnvironmentalStatusComponent* EnvComp = OtherActor->FindComponentByClass<UEnvironmentalStatusComponent>();
	if (EnvComp && OverlappingActors.Contains(EnvComp))
	{
		EnvComp->ModifyTemperature(-TemperatureDelta, ChangeSpeed);
		EnvComp->ModifyHumidity(-HumidityDelta, ChangeSpeed);

		OverlappingActors.Remove(EnvComp);
	}
}

