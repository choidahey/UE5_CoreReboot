#include "InteractableGimmick.h"

#include "Gimmick/Components/InteractableComponent.h"

AInteractableGimmick::AInteractableGimmick()
{
	PrimaryActorTick.bCanEverTick = false;

	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));

	/** SET AS AN INTERACTION TRACECHANNEL BLOCK */
	GetGimmickMeshComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
}

void AInteractableGimmick::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(InteractableComponent))
	{
		InteractableComponent->OnTryInteract.BindUObject(this, &ThisClass::OnGimmickInteracted);
	}
}

void AInteractableGimmick::OnGimmickInteracted()
{
	UE_LOG(LogTemp, Warning, TEXT("InteractableGimmick is interacted"));
}
