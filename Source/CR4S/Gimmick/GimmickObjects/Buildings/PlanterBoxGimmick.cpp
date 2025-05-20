#include "PlanterBoxGimmick.h"

#include "Gimmick/Components/InteractableComponent.h"
#include "Gimmick/GimmickObjects/Farming/GrowableGimmick.h"
#include "Gimmick/Manager/ItemGimmickSubsystem.h"
#include "Inventory/InventorySystemComponent.h"

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
		InteractableComponent->OnDetectionStateChanged.BindUObject(this, &ThisClass::OnDetectionStateChanged);
		InteractableComponent->OnTryInteract.BindUObject(this, &ThisClass::OnGimmickInteracted);
	}
}

void APlanterBoxGimmick::OnGimmickInteracted()
{
	if (!IsValid(InteractableComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("InteractableComponent is not valid"));
		return;
	}

	if (!IsValid(DetectingPlayerController))
	{
		UE_LOG(LogTemp, Warning, TEXT("DetectingPlayerController is not valid"));
		return;
	}

	if (IsHeldItemSeed())
	{
		UItemGimmickSubsystem* ItemGimmickSubsystem = GetGameInstance()->GetSubsystem<UItemGimmickSubsystem>();
		if (IsValid(ItemGimmickSubsystem))
		{
			PlantedGimmick
				= ItemGimmickSubsystem->SpawnGimmickByRowName<AGrowableGimmick>(TEXT("Crop"), SpawnPoint->GetComponentLocation());

			if (!IsValid(PlantedGimmick))
			{
				UE_LOG(LogTemp, Warning, TEXT("Gimmick is not valid"));
				return;
			}

			PlantedGimmick->OnHarvest.BindUObject(this, &ThisClass::OnHarvest);

			InteractableComponent->UpdateTraceBlocking(ECR_Ignore);
		}
	}
}

void APlanterBoxGimmick::OnDetectionStateChanged(APlayerController* InDetectingPlayerController,
                                                 const bool bInIsDetected)
{
	DetectingPlayerController = InDetectingPlayerController;
	bIsDetected = bInIsDetected;
}

void APlanterBoxGimmick::OnHarvest()
{
	PlantedGimmick = nullptr;
	InteractableComponent->UpdateTraceBlocking(ECR_Block);
}

bool APlanterBoxGimmick::IsHeldItemSeed() const
{
	if (!IsValid(DetectingPlayerController) || !bIsDetected)
	{
		UE_LOG(LogTemp, Warning, TEXT("DetectingPlayerController is not valid or bIsDetected is false"));
		return false;
	}

	const APawn* DetectingPawn = DetectingPlayerController->GetPawn();
	if (!IsValid(DetectingPawn))
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter is not valid"));
		return false;
	}
	
	const UInventorySystemComponent* InventorySystem
		= DetectingPawn->FindComponentByClass<UInventorySystemComponent>();

	if (!IsValid(InventorySystem))
	{
		UE_LOG(LogTemp, Warning, TEXT("InventorySystem is not valid"));
		return false;
	}

	return InventorySystem->GetCurrentHeldItemName() == FName("Seed");
}
