#include "CompleteGimmick.h"

#include "Gimmick/Components/DestructibleComponent.h"
#include "Gimmick/Components/InteractableComponent.h"

ACompleteGimmick::ACompleteGimmick()
{
	PrimaryActorTick.bCanEverTick = false;

	DestructibleComponent = CreateDefaultSubobject<UDestructibleComponent>(TEXT("DestructibleComponent"));
	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));
	
	/** SET AS AN INTERACTION TRACECHANNEL BLOCK */
	GetGimmickMeshComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
}

void ACompleteGimmick::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(DestructibleComponent))
	{
		DestructibleComponent->OnDestroy.BindUObject(this, &ThisClass::OnGimmickDestroy);
	}
	
	if (IsValid(InteractableComponent))
	{
		InteractableComponent->OnTryInteract.BindUObject(this, &ThisClass::OnGimmickInteracted);
	}
}

void ACompleteGimmick::OnGimmickDestroy()
{
	if (!bIsActorDestroyOnDestroyAction)
	{
		return;
	}

	if (DestroyDelay == 0.f)
	{
		DelayedDestroy();
	}

	GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &ThisClass::DelayedDestroy, DestroyDelay, false);
}

void ACompleteGimmick::DelayedDestroy()
{
	UE_LOG(LogTemp, Warning, TEXT("Gimmick is destroyed"));

	Destroy();
}

void ACompleteGimmick::OnGimmickInteracted()
{
	UE_LOG(LogTemp, Warning, TEXT("CompleteGimmick is interacted"));
}

