#include "WorkbenchGimmick.h"

#include "CR4S.h"
#include "Character/Characters/PlayerCharacter.h"
#include "Gimmick/Components/InteractableComponent.h"
#include "Inventory/Components/PlayerInventoryComponent.h"

AWorkbenchGimmick::AWorkbenchGimmick()
	: CraftingDifficulty(1)
{
	PrimaryActorTick.bCanEverTick = false;

	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));
}

void AWorkbenchGimmick::BeginPlay()
{
	Super::BeginPlay();

	if (CR4S_VALIDATE(LogGimmick, IsValid(InteractableComponent)))
	{
		InteractableComponent->OnTryInteract.BindDynamic(this, &ThisClass::OnGimmickInteracted);
	}
}

void AWorkbenchGimmick::OnGimmickInteracted(AActor* Interactor)
{
	const APlayerCharacter* Player = Cast<APlayerCharacter>(Interactor);
	if (!CR4S_VALIDATE(LogGimmick, IsValid(Player)))
	{
		return;
	}

	const UPlayerInventoryComponent* PlayerInventoryComponent = Player->FindComponentByClass<UPlayerInventoryComponent>();
	if (!CR4S_VALIDATE(LogGimmick, IsValid(PlayerInventoryComponent)))
	{
		return;
	}

	PlayerInventoryComponent->OpenPlayerInventoryWidget(1);
}
