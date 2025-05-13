#include "CompleteGimmick.h"

#include "Gimmick/Components/DestructibleComponent.h"
#include "Gimmick/Components/InteractableComponent.h"

ACompleteGimmick::ACompleteGimmick()
{
	PrimaryActorTick.bCanEverTick = false;

	DestructibleComponent = CreateDefaultSubobject<UDestructibleComponent>(TEXT("DestructibleComponent"));
	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));
	
	/** SET AS AN INTERACTION TRACECHANNEL BLOCK */
	GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
}

void ACompleteGimmick::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(DestructibleComponent))
	{
		DestructibleComponent->OnDestroy.BindUObject(this, &ThisClass::OnGimmickDestroyed);
	}
	
	if (IsValid(InteractableComponent))
	{
		InteractableComponent->OnTryInteract.BindUObject(this, &ThisClass::OnGimmickInteracted);
	}
}

void ACompleteGimmick::OnGimmickDestroyed()
{
	UE_LOG(LogTemp, Warning, TEXT("Gimmick is destroyed"));

	Destroy();
}

void ACompleteGimmick::OnGimmickInteracted()
{
	UE_LOG(LogTemp, Warning, TEXT("CompleteGimmick is interacted"));
}

