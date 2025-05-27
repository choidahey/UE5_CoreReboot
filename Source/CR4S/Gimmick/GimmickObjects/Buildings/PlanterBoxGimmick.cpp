#include "PlanterBoxGimmick.h"

#include "CR4S.h"
#include "Gimmick/Components/InteractableComponent.h"
#include "Gimmick/GimmickObjects/Farming/CropsGimmick.h"
#include "Gimmick/Manager/ItemGimmickSubsystem.h"

APlanterBoxGimmick::APlanterBoxGimmick()
{
	PrimaryActorTick.bCanEverTick = false;

	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));

	SpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPoint"));
	SpawnPoint->SetupAttachment(RootComponent);
	SpawnPoint->SetMobility(EComponentMobility::Movable);
}

void APlanterBoxGimmick::BeginPlay()
{
	Super::BeginPlay();

	if (CR4S_VALIDATE(LogGimmick, IsValid(InteractableComponent)))
	{
		InteractableComponent->OnDetectionStateChanged.BindDynamic(this, &ThisClass::OnDetectionStateChanged);
		InteractableComponent->OnTryInteract.BindDynamic(this, &ThisClass::OnGimmickInteracted);
	}
}

void APlanterBoxGimmick::OnGimmickDestroy(AActor* DamageCauser)
{
	if (CR4S_VALIDATE(LogGimmick, IsValid(PlantedGimmick)))
	{
		PlantedGimmick->Destroy();
		PlantedGimmick = nullptr;
	}

	Super::OnGimmickDestroy(DamageCauser);
}

void APlanterBoxGimmick::OnGimmickInteracted(AActor* Interactor)
{
	if (!CR4S_VALIDATE(LogGimmick, IsValid(Interactor)) ||
		!CR4S_VALIDATE(LogGimmick, IsValid(InteractableComponent)))
	{
		return;
	}

	UItemGimmickSubsystem* ItemGimmickSubsystem = GetGameInstance()->GetSubsystem<UItemGimmickSubsystem>();
	if (!CR4S_VALIDATE(LogGimmick, IsValid(ItemGimmickSubsystem)))
	{
		return;
	}

	PlantedGimmick = ItemGimmickSubsystem->SpawnGimmickByRowName<ACropsGimmick>(TEXT("Crops"),
		SpawnPoint->GetComponentLocation());

	if (!CR4S_VALIDATE(LogGimmick, IsValid(PlantedGimmick)))
	{
		return;
	}

	PlantedGimmick->OnHarvest.BindDynamic(this, &ThisClass::OnHarvest);
	InteractableComponent->UpdateTraceBlocking(ECR_Ignore);
}

void APlanterBoxGimmick::OnDetectionStateChanged(AActor* InDetectingActor,
                                                 const bool bInIsDetected)
{
	DetectingActor = InDetectingActor;
	bIsDetected = bInIsDetected;
}

void APlanterBoxGimmick::OnHarvest()
{
	PlantedGimmick = nullptr;
	
	if (!CR4S_VALIDATE(LogGimmick, InteractableComponent))
	{
		return;
	}
	InteractableComponent->UpdateTraceBlocking(ECR_Block);
}
