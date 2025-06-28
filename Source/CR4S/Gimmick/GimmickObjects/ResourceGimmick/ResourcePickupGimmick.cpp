#include "ResourcePickupGimmick.h"

#include "CR4S.h"
#include "Game/System/AudioManager.h"
#include "Gimmick/Components/InteractableComponent.h"
#include "Gimmick/Data/GimmickData.h"

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
		InteractableComponent->OnTryInteract.AddUniqueDynamic(this, &ThisClass::OnGimmickInteracted);
	}

	if (const FGimmickInfoData* GimmickInfoData = GetGimmickInfoData())
	{
		InteractSound = GimmickInfoData->InteractSound;
	}
}

void AResourcePickupGimmick::OnGimmickInteracted(AActor* Interactor)
{
	PlaySFX(InteractSound, GetActorLocation(), EConcurrencyType::Default);
	
	GetResources(Interactor);

	GimmickDestroy();
}


