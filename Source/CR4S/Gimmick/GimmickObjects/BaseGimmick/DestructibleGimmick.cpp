#include "DestructibleGimmick.h"

#include "CR4S/Gimmick/Components/DestructibleComponent.h"

ADestructibleGimmick::ADestructibleGimmick()
{
	PrimaryActorTick.bCanEverTick = false;

	DestructibleComponent = CreateDefaultSubobject<UDestructibleComponent>(TEXT("DestructibleComponent"));
}

void ADestructibleGimmick::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(DestructibleComponent))
	{
		DestructibleComponent->OnDestroy.BindUObject(this, &ThisClass::OnGimmickDestroyed);
	}
}

void ADestructibleGimmick::OnGimmickDestroyed()
{
	UE_LOG(LogTemp, Warning, TEXT("Gimmick is destroyed"));

	Destroy();
}

