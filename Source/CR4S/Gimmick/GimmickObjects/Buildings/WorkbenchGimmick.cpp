#include "WorkbenchGimmick.h"

#include "CR4S.h"
#include "Character/Characters/PlayerCharacter.h"
#include "Inventory/Components/PlayerInventoryComponent.h"

AWorkbenchGimmick::AWorkbenchGimmick()
	: CraftingDifficulty(1)
{
	PrimaryActorTick.bCanEverTick = false;
}

void AWorkbenchGimmick::OnGimmickInteracted(AActor* Interactor)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(Interactor);
	if (!CR4S_VALIDATE(LogGimmick, IsValid(Player)))
	{
		return;
	}

	UPlayerInventoryComponent* PlayerInventoryComponent = Player->FindComponentByClass<UPlayerInventoryComponent>();
	if (!CR4S_VALIDATE(LogGimmick, IsValid(PlayerInventoryComponent)))
	{
		return;
	}

	PlayerInventoryComponent->OpenPlayerInventoryWidget(1);
}
