#include "ResourcePickupGimmick.h"

#include "CR4S.h"
#include "Gimmick/Components/InteractableComponent.h"

AResourcePickupGimmick::AResourcePickupGimmick()
{
	PrimaryActorTick.bCanEverTick = false;

	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));
}

void AResourcePickupGimmick::BeginPlay()
{
	Super::BeginPlay();

	if (CR4S_VALIDATE(LogGimmick, IsValid(InteractableComponent)))
	{
		InteractableComponent->OnTryInteract.BindDynamic(this, &ThisClass::OnGimmickInteracted);
	}
}

void AResourcePickupGimmick::OnGimmickInteracted(AActor* Interactor)
{
	GetResources(Interactor);

	GimmickDestroy();
}


