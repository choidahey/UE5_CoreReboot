#include "PlanterBoxGimmick.h"

#include "Gimmick/Components/InteractableComponent.h"
#include "Inventory/InventorySystemComponent.h"

APlanterBoxGimmick::APlanterBoxGimmick()
{
	PrimaryActorTick.bCanEverTick = false;


	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));
	if (IsValid(InteractableComponent))
	{
		InteractableComponent->SetInteractionText(FText::FromString(TEXT("심기 가능")));
	}
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
		
		return;
	}
}

void APlanterBoxGimmick::OnDetectionStateChanged(APlayerController* InDetectingPlayerController,
                                                 const bool bInIsDetected)
{
	DetectingPlayerController = InDetectingPlayerController;
	bIsDetected = bInIsDetected;
}

bool APlanterBoxGimmick::IsHeldItemSeed() const
{
	if (!IsValid(DetectingPlayerController) || !bIsDetected)
	{
		UE_LOG(LogTemp, Warning, TEXT("DetectingPlayerController is not valid or bIsDetected is false"));
		return false;
	}

	const UInventorySystemComponent* InventorySystem
		= DetectingPlayerController->FindComponentByClass<UInventorySystemComponent>();

	if (!IsValid(InventorySystem))
	{
		UE_LOG(LogTemp, Warning, TEXT("InventorySystem is not valid"));
		return false;
	}

	return InventorySystem->GetCurrentHeldItemName() == FName("Seed");
}
