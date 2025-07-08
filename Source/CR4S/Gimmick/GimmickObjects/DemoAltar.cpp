#include "Gimmick/GimmickObjects/DemoAltar.h"
#include "Components/StaticMeshComponent.h"
#include "Gimmick/Components/InteractableComponent.h"
#include "Components/WidgetComponent.h" 
#include "UI/InGame/AltarWidget.h"
#include "Game/System/EnvironmentManager.h"
#include "MonsterAI/BossMonster/Season/SeasonBossMonster.h"
#include "Kismet/GameplayStatics.h"
#include "Gimmick/Components/InteractionComponent.h"

ADemoAltar::ADemoAltar()
{
	PrimaryActorTick.bCanEverTick = false;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = RootScene;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(RootComponent);
	WidgetComponent->SetWidgetClass(AltarWidgetClass);
	WidgetComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WidgetComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	WidgetComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));
}

void ADemoAltar::BeginPlay()
{
	Super::BeginPlay();

	InteractableComponent->OnTryInteract.AddDynamic(this, &ADemoAltar::HandleTryInteract);
	WidgetComponent->SetHiddenInGame(true);

	UUserWidget* RawWidget = WidgetComponent->GetUserWidgetObject();
	if (UAltarWidget* AltarWidget = Cast<UAltarWidget>(RawWidget))
	{
		AltarWidget->Altar = this;
	}

	EnvironmentManager = Cast<AEnvironmentManager>(
		UGameplayStatics::GetActorOfClass(GetWorld(), AEnvironmentManager::StaticClass()));
}

void ADemoAltar::HandleTryInteract(AActor* Interactor)
{
	if (!WidgetComponent || !InteractableComponent) return;

	if (!bIsActive)
	{
		UAltarWidget* AltarWidget = Cast<UAltarWidget>(WidgetComponent->GetUserWidgetObject());

		//if (AltarWidget)
		//{
		//	AltarWidget->ShowAltarUI();
		//}

		WidgetComponent->SetHiddenInGame(false);
		InteractableComponent->ActivateEndOverlapCollision();
		InteractableComponent->OnEndSphereOverlap.AddDynamic(this, &ADemoAltar::HandleEndInteract);
		InteractableComponent->UpdateTraceBlocking(ECollisionResponse::ECR_Ignore);

		bIsActive = true;
	}
}



void ADemoAltar::HandleEndInteract(AActor* Interactor)
{
	if (!WidgetComponent || !InteractableComponent) return;

	if (bIsActive)
	{
		UAltarWidget* AltarWidget = Cast<UAltarWidget>(WidgetComponent->GetUserWidgetObject());

		//if (AltarWidget)
		//{
		//	AltarWidget->HideAltarUI();
		//}

		WidgetComponent->SetHiddenInGame(true);
		InteractableComponent->OnEndSphereOverlap.RemoveAll(this);
		InteractableComponent->UpdateTraceBlocking();
		bIsActive = false;
	}
}

void ADemoAltar::SummonFrostBoss()
{
	UWorld* World = GetWorld();
	if (!World || !FrostBossClass) return;

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(World, 0);
	if (!PlayerCharacter) return;

	FVector PlayerLocation = PlayerCharacter->GetActorLocation();

	const float MinDistance = 1000.f;
	const float MaxDistance = 1500.f;
	const float SpawnHeightOffset = 100.f;

	FVector RandomOffset = FMath::VRand();
	RandomOffset.Z = 0.f;
	RandomOffset.Normalize();

	float RandomDistance = FMath::FRandRange(MinDistance, MaxDistance);
	FVector SpawnLocation = PlayerLocation + RandomOffset * RandomDistance;
	SpawnLocation.Z += SpawnHeightOffset;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ASeasonBossMonster* SpawnedBoss = World->SpawnActor<ASeasonBossMonster>(
		FrostBossClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

	if (SpawnedBoss)
	{
		UE_LOG(LogTemp, Log, TEXT("DemoAltar: Frost boss successfully spawned at %s"), *SpawnLocation.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("DemoAltar: Failed to spawn frost boss."));
	}
}

void ADemoAltar::RequestChangeWeather(ESeasonType Season)
{
	EnvironmentManager->SetWeatherBySeason(Season, TransitionTime);
}

