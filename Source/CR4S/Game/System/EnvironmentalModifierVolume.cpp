#include "Game/System/EnvironmentalModifierVolume.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Character/Components/EnvironmentalStatusComponent.h"
#include "CR4S.h"

AEnvironmentalModifierVolume::AEnvironmentalModifierVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneComponent;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->SetupAttachment(RootComponent);
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

#if WITH_EDITOR
	CapsuleComponent->SetHiddenInGame(false);
#endif

	CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnvironmentalModifierVolume::OnBeginOverlap);
	CapsuleComponent->OnComponentEndOverlap.AddDynamic(this, &AEnvironmentalModifierVolume::OnEndOverlap);
}


void AEnvironmentalModifierVolume::Initialize(float InRadius, float InHeight, float InTempDelta, float InHumidDelta, float InChangeSpeed, float InDuration)
{
	UE_LOG(LogEnvironment, Warning, TEXT("Initialize called with TempDelta: %.2f"), InTempDelta);

	Radius = InRadius;
	Height = InHeight;
	TemperatureDelta = InTempDelta;
	HumidityDelta = InHumidDelta;
	ChangeSpeed = InChangeSpeed;
	Duration = InDuration;

	CapsuleComponent->SetCapsuleSize(Radius, Height);

	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CapsuleComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	CapsuleComponent->SetGenerateOverlapEvents(true);

#if WITH_EDITOR
	DrawDebugCapsule(
		GetWorld(),
		CapsuleComponent->GetComponentLocation(),
		CapsuleComponent->GetScaledCapsuleHalfHeight(),
		CapsuleComponent->GetScaledCapsuleRadius(),
		CapsuleComponent->GetComponentRotation().Quaternion(),
		FColor::Green,
		false, 
		Duration, 
		0, 
		5.0f 
	);
#endif
}

void AEnvironmentalModifierVolume::BeginPlay()
{
	Super::BeginPlay();
}
void AEnvironmentalModifierVolume::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this) return;

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

	UEnvironmentalStatusComponent* EnvComp = OtherActor->FindComponentByClass<UEnvironmentalStatusComponent>();
	if (EnvComp && OverlappingActors.Contains(EnvComp))
	{
		EnvComp->ModifyTemperature(-TemperatureDelta, ChangeSpeed);
		EnvComp->ModifyHumidity(-HumidityDelta, ChangeSpeed);

		OverlappingActors.Remove(EnvComp);
	}
}
