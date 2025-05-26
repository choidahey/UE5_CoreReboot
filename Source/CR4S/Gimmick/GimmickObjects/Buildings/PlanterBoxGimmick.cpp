#include "PlanterBoxGimmick.h"

#include "CR4S.h"
#include "Gimmick/Components/InteractableComponent.h"
#include "Gimmick/GimmickObjects/Farming/CropsGimmick.h"
#include "Gimmick/Manager/ItemGimmickSubsystem.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/InventoryItem/BaseInventoryItem.h"

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

	if (IsValid(InteractableComponent))
	{
		InteractableComponent->OnDetectionStateChanged.BindDynamic(this, &ThisClass::OnDetectionStateChanged);
		InteractableComponent->OnTryInteract.BindDynamic(this, &ThisClass::OnGimmickInteracted);
	}
}

void APlanterBoxGimmick::OnGimmickDestroy(AActor* DamageCauser)
{
	if (IsValid(PlantedGimmick))
	{
		PlantedGimmick->Destroy();
		PlantedGimmick = nullptr;
	}

	Super::OnGimmickDestroy(DamageCauser);
}

void APlanterBoxGimmick::OnGimmickInteracted(AActor* Interactor)
{
	if (!IsValid(Interactor))
	{
		CR4S_Log(LogTemp, Warning, TEXT("Interactor is invalid"));
		return;
	}

	if (!IsValid(InteractableComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("InteractableComponent is invalid"));
		return;
	}

	UItemGimmickSubsystem* ItemGimmickSubsystem = GetGameInstance()->GetSubsystem<UItemGimmickSubsystem>();
	if (!IsValid(ItemGimmickSubsystem))
	{
		CR4S_Log(LogTemp, Warning, TEXT("ItemGimmickSubsystem is invalid"));
		return;
	}

	PlantedGimmick = ItemGimmickSubsystem->SpawnGimmickByRowName<ACropsGimmick>(TEXT("Crops"),
		SpawnPoint->GetComponentLocation());

	if (!IsValid(PlantedGimmick))
	{
		UE_LOG(LogTemp, Warning, TEXT("Gimmick is invalid"));
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
	InteractableComponent->UpdateTraceBlocking(ECR_Block);
}
