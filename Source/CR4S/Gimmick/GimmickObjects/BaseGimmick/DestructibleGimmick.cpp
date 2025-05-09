#include "BaseResourceGimmick.h"

#include "CR4S/Gimmick/Components/DestructibleComponent.h"

ABaseResourceGimmick::ABaseResourceGimmick()
{
	PrimaryActorTick.bCanEverTick = false;

	DestructibleComponent = CreateDefaultSubobject<UDestructibleComponent>(TEXT("DestructibleComponent"));
}

void ABaseResourceGimmick::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(DestructibleComponent))
	{
		DestructibleComponent->OnDestroyed.BindUObject(this, &ThisClass::OnDestroyedGimmick);
	}
}

void ABaseResourceGimmick::OnDestroyedGimmick()
{
	UE_LOG(LogTemp, Warning, TEXT("Gimmick Destroy!"));

	Destroy();
}

