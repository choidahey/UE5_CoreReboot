#include "PlanterBoxGimmick.h"

#include "CR4S.h"
#include "Gimmick/Components/InteractableComponent.h"
#include "Gimmick/GimmickObjects/Farming/CropsGimmick.h"
#include "Gimmick/Manager/ItemGimmickSubsystem.h"
#include "Inventory/InventoryType.h"
#include "Inventory/Components/PlanterBoxInventoryComponent.h"
#include "Inventory/Components/PlayerInventoryComponent.h"

APlanterBoxGimmick::APlanterBoxGimmick()
{
	PrimaryActorTick.bCanEverTick = false;

	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));

	SpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPoint"));
	SpawnPoint->SetupAttachment(RootComponent);
	SpawnPoint->SetMobility(EComponentMobility::Movable);

	PlanterBoxInventoryComponent = CreateDefaultSubobject<UPlanterBoxInventoryComponent>(TEXT("InventoryComponent"));
	PlanterBoxInventoryComponent->SetMaxInventorySize(1);
}

void APlanterBoxGimmick::BeginPlay()
{
	Super::BeginPlay();

	if (CR4S_VALIDATE(LogGimmick, IsValid(InteractableComponent)))
	{
		InteractableComponent->OnDetectionStateChanged.BindDynamic(this, &ThisClass::OnDetectionStateChanged);
		InteractableComponent->OnTryInteract.BindDynamic(this, &ThisClass::OnGimmickInteracted);
	}

	if (CR4S_VALIDATE(LogGimmick, IsValid(PlanterBoxInventoryComponent)))
	{
		PlanterBoxInventoryComponent->OnItemSlotChanged.AddUniqueDynamic(this, &ThisClass::HandlePlantingCropsGimmick);
	}
}

void APlanterBoxGimmick::OnGimmickDestroy(AActor* DamageCauser)
{
	if (CR4S_VALIDATE(LogGimmick, IsValid(PlantedGimmick)))
	{
		PlantedGimmick->Destroy();
		PlantedGimmick = nullptr;
	}
	
	if (IsValid(InteractableComponent))
	{
		InteractableComponent->UpdateTraceBlocking(ECR_Ignore);
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

	UPlayerInventoryComponent* PlayerInventoryComponent = Interactor->FindComponentByClass<UPlayerInventoryComponent>();
	if (!CR4S_VALIDATE(LogGimmick, IsValid(PlayerInventoryComponent)))
	{
		return;
	}

	PlayerInventoryComponent->OpenOtherInventoryWidget(EInventoryType::PlantBox, PlanterBoxInventoryComponent);
}

void APlanterBoxGimmick::OnDetectionStateChanged(AActor* InDetectingActor,
                                                 const bool bInIsDetected)
{
	DetectingActor = InDetectingActor;
	bIsDetected = bInIsDetected;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void APlanterBoxGimmick::HandlePlantingCropsGimmick(const int32 SlotIndex, UBaseInventoryItem* Item)
{
	if (!CR4S_VALIDATE(LogGimmick, IsValid(ItemGimmickSubsystem)) ||
		!CR4S_VALIDATE(LogGimmick, IsValid(Item)))
	{
		return;
	}

	const FName RowName = Item->GetInventoryItemData()->RowName;
	PlantedGimmick = ItemGimmickSubsystem->SpawnGimmickByRowName<ACropsGimmick>(RowName,
		SpawnPoint->GetComponentLocation(), GetActorRotation());

	if (!CR4S_VALIDATE(LogGimmick, IsValid(PlantedGimmick)))
	{
		return;
	}

	PlantedGimmick->OnHarvest.AddUniqueDynamic(this, &ThisClass::HandleHarvest);
	PlantedGimmick->OnCropComposted.AddUniqueDynamic(this, &ThisClass::HandleHarvest);
}

void APlanterBoxGimmick::HandleHarvest()
{
	PlantedGimmick = nullptr;
	
	if (!CR4S_VALIDATE(LogGimmick, IsValid(InteractableComponent)) ||
		!CR4S_VALIDATE(LogGimmick, IsValid(PlanterBoxInventoryComponent)))
	{
		return;
	}
	PlanterBoxInventoryComponent->RemoveItemByIndex(0, -1);
}
